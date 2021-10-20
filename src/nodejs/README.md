# jim-lite/nodejs
This part is the backend of face detecting. `faceDB` is the database of signed up faces, `out` is the detecting result of every detections.

## Startup
```sh
ts-node app.ts
```

## Sign Up
```js
let face = 'data:image/jpg;base64,/9j/4AAQSkZJRgABAQEA...'; // base64 encoded face picture
let username = '小飞';
let package = 'signIn';
let res = await fetch('http://127.0.0.1:3000/',{
  method:'post',
  body: JSON.stringify({package, username, face}),
});
await res.text();
```

## Sign In
```js
let face = 'data:image/jpg;base64,/9j/4AAQSkZJRgABAQEA...'; // base64 encoded face picture
let package = 'signIn';
let res = await fetch('http://127.0.0.1:3000/',{
  method:'post',
  body: JSON.stringify({package, face}),
});
await res.text();
```