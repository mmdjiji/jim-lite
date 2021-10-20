# Protocol

In client view.

在协议中，默认任何人之间互为好友，因此，不需要加好友包。

---

## 登录 (01)

### 请求
```json
{"package": "signIn", "username": "<username>","password": "<password>"}
```

### 应答

登录成功时:
```json
{"package": "signIn", "result": "success", "username": "<username>", "nickname": "<nickname>", "token": "<token>"}
```

登录失败时:
```json
{"package": "signIn", "result": "failed", "reason": "<reason>"}
```
---

## 注册 (02)

### 请求
```json
{"package": "signUp", "username": "<username>", "password": "<password>", "nickname": "<nickname>", "face": "<face_base64>"}
```
注：其中 `<face_base64>` 为可选人脸信息。

### 应答
注册成功时:
```json
{"package": "signUp", "result": "success"}
```

注册失败时:
```json
{"package": "signUp", "result": "failed", "reason": "<reason>"}
```
---

## 获取好友列表 (03)

### 请求
```json
{"package": "getFriendsList", "token": "<token>"}
```

### 应答
验证成功时:
```json
{"package": "getFriendsList", "result": "success", "friends": [{"username": "<username_1>", "nickname": "<nickname_1>"}, {"username": "<username_2>", "nickname": "<nickname_2>"}]}
```

验证失败时:
```json
{"package": "getFriendsList", "result": "failed", "reason": "<reason>"}
```
---

## 获取群列表 (04)

### 请求
```json
{"package": "getGroupsList", "token": "<token>"}
```

### 应答
验证成功时:
```json
{"package": "getGroupsList", "result": "success", "groups": [{"id": "<group_id_1>", "nickname": "<nickname_1>"}, {"id": "<group_id_2>", "nickname": "<nickname_2>"}]}
```

验证失败时:
```json
{"package": "getGroupsList", "result": "failed", "reason": "<reason>"}
```
---

## 获取群成员列表 (05)

### 请求
```json
{"package": "getGroupMembers", "token": "<token>", "id": "<group_id>"}
```

### 应答
验证成功时:
```json
{"package": "getGroupMembers", "result": "success", "owner": {"username": "<username>", "nickname": "<nickname>"}, "members": [{"username": "<username_1>", "nickname": "<nickname_1>"}, {"username": "<username_2>", "nickname": "<nickname_2>"}]}
```

验证失败时:
```json
{"package": "getGroupMembers", "result": "failed", "reason": "<reason>"}
```
---

## 发送好友消息 (06)

### 请求
```json
{"package": "sendFriendMessage", "token": "<token>", "message": {"dst": "<friend_username>", "type": "plain", "text": "<text>"}}
```

### 应答
验证成功时:
```json
{"package": "sendFriendMessage", "result": "success"}
```

验证失败时:
```json
{"package": "sendFriendMessage", "result": "failed", "reason": "<reason>"}
```
---

## 发送群消息 (07)

### 请求
```json
{"package": "sendGroupMessage", "token": "<token>", "message": {"dst": "<group_id>", "type": "plain", "text": "<text>"}}
```

### 应答
验证成功时:
```json
{"package": "sendGroupMessage", "result": "success"}
```

验证失败时:
```json
{"package": "sendGroupMessage", "result": "failed", "reason": "<reason>"}
```
---

## 接收好友消息 (08)

### 应答
```json
{"package": "recvFriendMessage", "message": {"time": "<time>", "src": "<friend_username>", "type": "plain", "text": "<text>"}}
```
注意: time是在加入消息队列前封装的。

---

## 接收群消息 (09)

### 应答
```json
{"package": "recvGroupMessage", "time": "<time>", "src": "<member_username>", "message": {"type": "plain", "text": "<text>"}}
```
---

## 创建群 (10)

### 请求
```json
{"package": "newGroup", "token": "<token>", "nickname": "<group_name>"}
```

### 应答
创建成功时:
```json
{"package": "newGroup", "result": "success"}
```

创建失败时:
```json
{"package": "newGroup", "result": "failed", "reason": "<reason>"}
```
---

## 申请添加群 (11)

### 请求
```json
{"package": "applyAddGroup", "token": "<token>", "id": "<group_id>"}
```

### 应答
成功时:
```json
{"package": "applyAddGroup", "result": "success"}
```

失败时:
```json
{"package": "applyAddGroup", "result": "failed", "reason": "<reason>"}
```
---

## 同意添加群 (12)

### 请求
```json
{"package": "verifyAddGroup", "token": "<token>", "username": "<username>", "id": "<group_id>"}
```

### 应答
成功时:
```json
{"package": "verifyAddGroup", "result": "success"}
```

失败时:
```json
{"package": "verifyAddGroup", "result": "failed", "reason": "<reason>"}
```

## 人脸登录 (13)

### 请求
```json
{"package": "faceSignIn", "face": "<face_base64>"}
```

## 应答
同 `登录 (01)` 的应答