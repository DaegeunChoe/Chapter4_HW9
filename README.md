# Chapter4 HW9 숫자 야구

내일배움캠프 Unreal, Chapter 4 과제 9번 제출용 저장소입니다.

[채점 자가진단 (블로그 글, 클릭시 이동)](https://15danpancake.atlassian.net/wiki/spaces/cdg1212/pages/91324423/Chapter+4+9)


## Environment

Windows x64, Unreal Engine 5.5.4


## Youtube

[![ALTTEXT](https://img.youtube.com/vi/GCOOdHE8RN8/0.jpg)](https://www.youtube.com/watch?v=GCOOdHE8RN8)


## 추가로 구현한 것

로비 시스템을 구현했습니다.
서버에 호스트와 게스트가 들어와 숫자 야구 게임을 할 수 있는 기존 코드에서 벗어나, 
로비와 게임 방 시스템을 만들어, 동시에 여러 개의 게임을 진행할 수 있도록 했습니다.


## Client UI 설명

로그인 화면입니다. 서버의 IP 주소와 포트 번호, 그리고 내가 사용할 NickName을 입력해야 합니다. Login 버튼을 누르면, 서버에 접속을 시도합니다.

<img width="790" height="590" alt="image" src="https://github.com/user-attachments/assets/c20d7719-6886-4f80-9102-8367d91a63fa" />


서버에 성공적으로 접속하면, 로비 UI가 나타납니다. 여기에서는 방 목록을 확인하거나 유저 목록을 확인할 수 있고, 로비에 있는 다른 유저와 채팅을 할 수 있습니다.
숫자 야구 게임을 하기 위해 방을 만들 수 있습니다.

<img width="781" height="588" alt="image" src="https://github.com/user-attachments/assets/d73fe067-32a2-4f37-8bff-1a9281c83c3c" />

방을 만들면 게임 화면으로 넘어갑니다. 누가 호스트이고 누가 게스트인지 확인할 수 있으며, 방 안에 있는 유저와 대화할 수 있습니다.

<img width="776" height="577" alt="image" src="https://github.com/user-attachments/assets/ef979020-e9d2-4097-818f-68addd9736ff" />

게스트가 들어오면 게임을 시작할 수 있습니다.

<img width="785" height="590" alt="image" src="https://github.com/user-attachments/assets/c457adce-24ae-418e-8f56-91a2f471f9ef" />

게임이 시작되면, 턴을 주고 받으며 숫자를 추측할 수 있습니다.

<img width="784" height="582" alt="image" src="https://github.com/user-attachments/assets/856aa6cc-5827-44c9-9f60-7e9b950ea236" />
<img width="788" height="586" alt="image" src="https://github.com/user-attachments/assets/88604805-97be-4c57-8b1f-6bd5887eeda7" />




## Files

### Contents
- `Content/NumberBaseball/`
  * `Blueprints/`
    - `Core/`
    - `Player/`
  * `Maps/`
    - `LoginMap`: (클라이언트 시작 맵)
    - `GameRoom` (서버 기본 맵, 게임이 이루어지는 레벨)
  * UI/
    - `WBP_GameRoom`: 숫자야구 게임을 할 수 있는 게임 방 UI
    - `WBP_Lobby`: 방 생성, 방 목록 확인, 접속 플레이어 확인, 전체 채팅이 가능한 로비 UI
    - `WBP_Login`: 서버에 접속하기 전, 서버 IP와 닉네임을 입력할 수 있는 로그인 UI
    - `WBP_RoomItem`: 방 목록에 사용하는 목록 아이템 UI


### Source Codes

- `Source/NumberBaseBall/`
  * `Core/`
    - `NBGameModeBase.cpp`: 접속 관리 및 숫자 야구 게임 룰을 관리하는 **GameMode**
    - `NBGameStateBase.cpp`: FGameRoom을 관리하는 **GameState**
    - `NBGameRoom.cpp`: 호스트와 게스트가 1:1로 숫자 야구 게임을 할 수 있는 방을 나타내는 클래스
  * `Player/Main/`
    - `NBPlayerController.cpp`: 게임 플레이 전용 **Player Controller**. UW_Lobby(로비 UI)와 UW_GameRoom(게임 UI)를 관리한다.
    - `NBPlayerController_ClientRPC.cpp`: 클라이언트 RPC 함수 정의 모음
    - `NBPlayerController_ServerRPC.cpp`: 서버 RPC 함수 정의 모음
    - `NBPlayerState.cpp`: 플레이어가 로비에 있는지 게임 방에 있는지, 게임 중이라면 남아있는 횟수는 몇인지, 남아있는 시간은 몇 초인지, 지금 자신의 차례인지 등을 저장하는 **Player State**
  * `Player/Login/`
    - `NBLoginPlayerController.cpp`: 로그인 전용 Player Controller
  * `UI/`
    - `UW_GameRoom.cpp`: 채팅 창, 시간 타이머 등이 있는 게임 방 UI, `WBP_GameRoom` 부모 클래스
    - `UW_Lobby.cpp`: 방 목록, 채팅 창, 유저 목록이 있는 게임 로비 UI, `WBP_Lobby` 부모 클래스
    - `UW_Login.cpp`: `WBP_Login` 부모 클래스
    - `IW_RoomItem.cpp`: `WBP_RoomItem` 부모 클래스



## Details

### (1) Login 

- `ANBLoginPlayerController::JoinServer()` 에서 서버 IP와 닉네임을 이용해 서버에 접속을 시도한다.
- `UGameplayStatics::OpenLevel()` 함수에 Option 문자열을 전달해서, 자신의 닉네임을 서버에게 전달한다.
- 접속에 성공하면, Option의 문자열을 파싱해 PlayerState에 닉네임을 저장하고, 접속한 플레이어를 로비로 보낸다.
   * Login
     * `ANBGameModeBase::InitPlayerStateUsingOptions()`
     * `ANBPlayerState::SetNickName()`
     * `ANBPlayerState::SetPlayerLocationToLobby()`
   * Post Login
     * `ANBGameModeBase::SendNotificationToLobby()`


### (2) 로비 화면

- 로비 화면에서는 현제 게임의 방 목록과, 플레이어 목록을 확인할 수 있다.
- 방 목록은 `TArray<FGameRoom> ANBGameStateBase::GameRooms` 에서 확인할 수 있으며, 이 값은 서버로부터 replicated되는 값이다.
- 플레이어 목록은 `TArray<TObjectPtr<APlayerState>> AGameStateBase::PlayerArray`에서 확인할 수 있다. 이 값은 클라이언트에 PlayerState가 생성되거나 제거될 때 자동으로 관리된다.


### (3) 채팅

- EditableTextBox에 메시지를 입력하고 Commit하면, `ANBPlayerController::ServerRPCSendChatMessage()`를 호출(Invoke)한다.
- 플레이어가 로비에 있다면, 로비에 있는 사람들에게 메시지를 보낸다.
- 플레이어가 게임 방 안에 있다면, 게임 방 안에 있는 사람들에게만 메시지를 보낸다.
- 플레이어게에 메시지를 보내는 함수는 `ANBPlayerController::ClientRPCReceiveChatMessage()` 이다.


### (4) 방 생성

- `ANBPlayerController::ServerRPCMakeRoom()`
- `ANBGameModeBase::MakeRoom()`
- 방을 만든 플레이어는 호스트가 된다.
- 방을 만든 후, 호스트 플레이어의 UI를 전환한다.
- `ANBPlayerController::ClientRPCMakeRoom()`


### (5) 방 입장

- `ANBPlayerController::ServerRPCJoinRoom()`
- `ANBGameModeBase::JoinRoom()`
- 방에 들어간 플레이어는 게스트가 된다.
- 그 방에 게스트가 있거나, 게임 도중이라면 방에 들어갈 수 없다.
- 방에 들어간 후, 게스트 플레이어의 UI를 전환한다.


### (6) 게임 시작

- 호스트 플레이어만 게임을 시작할 수 있다.
- 게스트가 들어와 있으면, 게임 시작 버튼이 나타난다.
- `ANBPlayerController::ServerRPCStartGame()`
- `ANBGameModeBase::StartGame()`
- GameMode에서 랜덤한 숫자를 생성하고, 타이머를 설정한다.
- `ANBGameModeBase::GenerateRandomNumberString()`
- 첫 턴을 결정한다. 호스트와 게스트는 동일한 확률로 첫 턴을 잡을 수 있다.


### (7) 게임 진행 관리

- 게임 타이머는 정해진 간격(기본값: 1초)마다 타이머 함수를 호출한다.
- `ANBGameModeBase::OnGameTimerElapsed()`
- 시간이 다 되면 턴을 넘기거나, 게임을 끝낸다. (이 경우, 무승부)
- 플레이어가 숫자를 추측하면, 서버에서 그 값을 확인한다.
- `ANBPlayerController::ServerRPCSendGuessNumber()`
- `ANBGameModeBase::GuessNumber()`
- 자신의 차례가 아니라면, 서버에서 거절한다.
- 자신의 차례라면, 추측한 숫자가 정답인지 확인 `ANBGameModeBase::JudgeResult()`
- 숫자를 추측하면, 턴을 상대방에게 넘긴다.
- 숫자를 맞췄다면, 게임을 끝낸다.


### (8) 방 나가기

- 게스트는 방을 그냥 나갈 수 있다.
- 호스트가 방을 나가면, 그 방을 파괴한다.
- `ANBPlayerController::ServerRPCLeaveRoom()`
- `ANBGameModeBase::LeaveRoom()`




