//@ts-nocheck
// Author: mmdjiji (JiJi)
// Github: https://github.com/mmdjiji/jim-lite

const express = require('express');
const app = express();
const url = require('url');
const http = require('http').Server(app);
const fs = require('fs');

const host = '0.0.0.0'; // 地址
const port = 4000; // 端口

import * as faceapi from 'face-api.js';

import { canvas, faceDetectionNet, faceDetectionOptions, saveFile } from './commons';

let faceMatcher = null;
let users = []; // 【注册】 人脸录入 可以push多个人脸 实现多人识别

let base64_encode = str=>Buffer.from(str).toString('base64');
let base64_decode = base64str=>Buffer.from(base64str, 'base64').toString();

async function run() {

  await faceDetectionNet.loadFromDisk('./weights'); // 载入基础模型 查找人脸目标
  await faceapi.nets.faceLandmark68Net.loadFromDisk('./weights'); // 载入绘制人脸的模型
  await faceapi.nets.faceRecognitionNet.loadFromDisk('./weights'); // 载入专门用来匹配人名的模型

  let init = async () => {
    let arr = fs.readdirSync('./faceDB');
    for(let i of arr) {
      const referenceImage = await canvas.loadImage('./faceDB/'+i); // 参考图像
      const label = base64_decode(i.substring(0, i.length-4));
      const aystRef = await faceapi.detectSingleFace(referenceImage, faceDetectionOptions).withFaceLandmarks().withFaceDescriptor(); // 一个人脸
      users.push(new faceapi.LabeledFaceDescriptors(label, [aystRef.descriptor]));
      faceMatcher = new faceapi.FaceMatcher(users); // 人脸识别管理器
      console.log(`loaded: ${label}`);
    }
  };
  init();

  app.use((req, res)=>{
    let str = '';
    req.on('data',(data)=>{
      str += data;
    });
    req.on('end', async (data) => {
      await faceProcess(res, str);
      res.end('');
    });
  });

  /**
   * 
   * @param res (package, face, username)
   * @param data (the body of request)
   * @returns 
   */

  let faceProcess = async (res, data) => {
    try{
      data = JSON.parse(data);
      //console.log(data);
    }catch(e){
      return;
    }
  
    if(!data.face || !data.package) {
      res.send('ok');
      console.log('Invalid param.');
      return;
    }

    if(data.username && data.package == 'signUp') {
      const referenceImage = await canvas.loadImage(data.face); // 参考图像
      const outRef = faceapi.createCanvasFromMedia(referenceImage); // 创建一个Canvas画布，把照片绘制上去。
      saveFile('../faceDB/'+base64_encode(data.username)+'.jpg', outRef.toBuffer('image/jpeg')); // 文件名尽量用英文
      const aystRef = await faceapi.detectSingleFace(outRef, faceDetectionOptions).withFaceLandmarks().withFaceDescriptor(); // 一个人脸
      users.push(new faceapi.LabeledFaceDescriptors(data.username, [aystRef.descriptor]));
      faceMatcher = new faceapi.FaceMatcher(users); // 人脸识别管理器
      console.log('reference done.');
    } else if(data.package == 'signIn') {
      const queryImage = await canvas.loadImage(data.face)         // 查找图像
      const outQuery = faceapi.createCanvasFromMedia(queryImage); // 创建一个Canvas画布，把照片绘制上去。
      const resultsQuery = await faceapi.detectAllFaces(outQuery, faceDetectionOptions).withFaceLandmarks().withFaceDescriptors(); // 查找的时候可以从多个人中查找 不认识的陌生人会返回unknown
      let bestMatch = null;
      const queryDrawBoxes = resultsQuery.map(res => { // 这里是为了遍历找到最匹配的那个人脸
        bestMatch = faceMatcher.findBestMatch(res.descriptor); // 返回匹配目标的人脸
        console.log(bestMatch);
        return new faceapi.draw.DrawBox(res.detection.box, {label: bestMatch.toString()}); // 在原图上给匹配的人脸画上标签（人名）
      });
      if(!bestMatch) {
        console.log('no best match');
        return;
      }
      queryDrawBoxes.forEach(drawBox => drawBox.draw(outQuery)) // 在人脸上绘制方框（包含标签）
      saveFile('./'+bestMatch.toString()+'.jpg', outQuery.toBuffer('image/jpeg')); // 文件名尽量用英文
      console.log('Query done.') // 输出处理完成的信息
      res.send(JSON.stringify({bestMatch}));
    }
    //res.end('login is ok');
  };
}
run();

http.listen(port, host, ()=>{
  console.log(`服务器运行在：http://${host}:${port}`);
});
