# RPi Obstacle Dodge Game
라즈베리 파이(Raspberry Pi)에서 동작하는 장애물 피하기 아케이드 게임입니다.
C언어로 작성되었으며 pigpio와 wiringPi 라이브러리를 사용하여 하드웨어(LCD, ADC, GPIO)를 직접 제어합니다.

# Features
Game Logic: 위에서 떨어지는 장애물을 피하는 서바이벌 방식
Controls: 아날로그 조이스틱(MCP3008)을 이용한 부드러운 이동
Display: ST7735 1.8인치 TFT LCD (SPI 통신)
Indicators: 3색 LED를 이용한 실시간 체력(Life) 표시
Audio: Pthread를 활용한 백그라운드 BGM 재생 (라운드 진행 시 템포 가속)

# Hardware Setup
라즈베리 파이 GPIO 핀 연결 정보입니다. (pins.h 기준)
Component    |Pin/Protocol  |Note
LCD (ST7735) |SPI0 (CE0)    | MOSI, SCLK
LCD D/C      |GPIO 5
LCD Reset    |GPIO 12 
ADC(MCP3008) |SPI1 (CE0)    |Joystick X-axis -> CH0
LED (Red)    |GPIO 17       |Life 1
LED (Yellow) |GPIO 27       |Life 2
LED (Green)  |GPIO 22       |Life 3
Buzzer       |GPIO 25       |Active Buzzer
Button       |GPIO 24       |Restart (Pull-up)

Note: LCD는 SPI0, ADC는 SPI1 채널을 사용합니다. raspi-config에서 SPI 인터페이스가 활성화되어 있어야 합니다.

# Dependencies
빌드 및 실행을 위해 아래 라이브러리가 필요합니다.
# pigpio 설치
sudo apt-get update
sudo apt-get install pigpio

# wiringPi 설치 (필요 시)
sudo apt-get install wiringpi


# Build & Run
Makefile이 포함되어 있어 바로 빌드 가능합니다.
# 컴파일
make

# 실행 (GPIO 제어를 위해 root 권한 필요)
sudo ./game

# 빌드 파일 정리
make clean

# How to Play
Start: 프로그램을 실행하면 게임이 시작됩니다.
Move: 조이스틱을 좌우로 움직여 초록색 플레이어를 이동시킵니다.
Life: 장애물 충돌 시 LED가 하나씩 꺼지며, 3번 충돌하면 게임 오버됩니다.
Restart: Game Over 화면에서 버튼을 누르면 재시작합니다.

# File Structure
main.c: 메인 루프, 하드웨어 초기화 및 이벤트 처리
game.c: 게임 로직(충돌 감지, 스코어 계산), 렌더링 함수
lcd.c: ST7735 LCD 드라이버 (SPI 커맨드 전송)
mcp3008.c: ADC 데이터 리딩 (SPI 통신)
music.c: 스레드 기반 BGM 재생 및 피치/속도 제어
pins.h: GPIO 핀 매핑 정의
