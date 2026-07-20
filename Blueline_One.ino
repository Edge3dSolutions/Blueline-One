#include <Arduino.h>
#include <Wire.h>
#include <Preferences.h>
#include <math.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEClient.h>

#include <lvgl.h>

#include "Arduino_GFX_Library.h"
#include "pin_config.h"
#include "TouchDrv.hpp"

#include <XPowersLib.h>
#define BLUELINE_HAS_XPOWERS 1

// =====================================================
// BlueLine One LVGL Clean v134
// =====================================================
// Updates:
// - Battery icon white dashboard style
// - Battery fill uses 1/4 segments
// - Fill color changes red / yellow / green by voltage
// - Battery icon moved up
// - Battery title 32 font
// - Battery voltage 72 font, lowercase v
// - Speed title 32 font
// - Speed number 72 font
// - MPH/KPH 32 font
// - RPM title 32 font, always white
// - RPM number 72 font, always white
// - BT hold 3 sec = BLE menu
// - Gear hold 3 sec = shift setup
// - MPH/KPH hold 3 sec = unit switch
// - Tap +/- = 25 RPM
// - Hold +/- = 250 RPM repeat
// - BLE scan menu shows top scored devices
// - Tap scan result to connect
// - Live OBD polling for battery voltage, RPM, and speed
// - No demo data; gauges show dashes when disconnected
// - Demo values removed: gauges show placeholders until live OBD data arrives
// - BLE main menu uses 2x2 circular buttons
// - Added Intake Air Temperature gauge
// - Added MAF / Airflow gauge
// - Added coolant temperature gauge using OBD PID 0105
// - Coolant temp unit toggles Fahrenheit/Celsius by 3-sec hold
// - RPM placeholder ---- uses gray when inactive/disconnected
// - Shift setup uses 3x2 circular buttons
// - FORGET shows NO SAVED DEVICE / SCAN FIRST if nothing is saved
// - After FORGET, BLE status returns to CONNECTED/DISCONNECTED instead of DEVICE REMOVED
// =====================================================

// =====================================================
// Fonts
// =====================================================

LV_FONT_DECLARE(Xolonium_pn4D12pt7b);
LV_FONT_DECLARE(Xolonium_pn4D18pt7b);
LV_FONT_DECLARE(Xolonium_pn4D24pt7b);
LV_FONT_DECLARE(Xolonium_pn4D32pt7b);
LV_FONT_DECLARE(Xolonium_pn4D48pt7b);
LV_FONT_DECLARE(Xolonium_pn4D60pt7b);
LV_FONT_DECLARE(Xolonium_pn4D72pt7b);

// =====================================================
// Display
// =====================================================

Arduino_DataBus *bus = new Arduino_ESP32QSPI(
  LCD_CS,
  LCD_SCLK,
  LCD_SDIO0,
  LCD_SDIO1,
  LCD_SDIO2,
  LCD_SDIO3
);

Arduino_CO5300 *gfx = new Arduino_CO5300(
  bus,
  LCD_RESET,
  0,
  LCD_WIDTH,
  LCD_HEIGHT,
  6, 0, 0, 0
);

// =====================================================
// LVGL
// =====================================================

static lv_display_t *display;
static lv_color_t draw_buf[LCD_WIDTH * 20];

lv_obj_t *screenRoot = nullptr;
lv_obj_t *ringLayer = nullptr;
lv_obj_t *contentLayer = nullptr;

// =====================================================
// Preferences
// =====================================================

Preferences prefs;

// =====================================================
// Touch Mapping
// =====================================================

#define TOUCH_SWAP_XY false
#define TOUCH_FLIP_X  true
#define TOUCH_FLIP_Y  true

// =====================================================
// Touch
// =====================================================

TouchDrvCST92xx touch;
bool touchReady = false;

bool touchActive = false;
bool longPressTriggered = false;
bool pendingTouchRelease = false;

int touchStartX = 0;
int touchStartY = 0;
int touchLastX = 0;
int touchLastY = 0;

unsigned long touchStartTime = 0;
unsigned long lastTouchSeenTime = 0;
unsigned long lastUserTouchActivityTime = 0;

const unsigned long OBD_TOUCH_PAUSE_TIME = 900;
bool obdCommandAbortedForTouch = false;

const int SWIPE_MIN_DISTANCE = 30;
const unsigned long SWIPE_MAX_TIME = 1200;
const unsigned long TOUCH_RELEASE_GRACE = 180;

#define LONG_TARGET_NONE  0
#define LONG_TARGET_BT    1
#define LONG_TARGET_GEAR  2
#define LONG_TARGET_UNIT  3
#define LONG_TARGET_COOLANT_UNIT 4
#define LONG_TARGET_INTAKE_AIR_UNIT 5
#define LONG_TARGET_OIL_UNIT 6
#define LONG_TARGET_MPG_RESET 7
#define LONG_TARGET_TPMS_UNIT 8

int longPressTarget = LONG_TARGET_NONE;

const unsigned long LONG_PRESS_TIME = 3000;
const int LONG_PRESS_MOVE_LIMIT = 90;

// Shift setup repeat buttons
bool repeatButtonActive = false;
int repeatButtonType = 0;
unsigned long repeatButtonLastTime = 0;

#define REPEAT_NONE  0
#define REPEAT_MINUS 1
#define REPEAT_PLUS  2

const unsigned long REPEAT_BUTTON_DELAY = 450;
const unsigned long REPEAT_BUTTON_INTERVAL = 130;
const int SHIFT_RPM_HOLD_STEP = 250;

// =====================================================
// Screens
// =====================================================

#define SCREEN_BATTERY        0
#define SCREEN_ALL_IN_ONE     13
#define SCREEN_RPM            1
#define SCREEN_SPEED          2
#define SCREEN_COOLANT        3
#define SCREEN_THROTTLE       4
#define SCREEN_INTAKE_AIR     5
#define SCREEN_AIRFLOW        6
#define SCREEN_MPG            7
#define SCREEN_CHRONO         8
#define SCREEN_TPMS           9
#define SCREEN_OIL_TEMP       10  // parked for future use; not in normal swipe rotation
#define SCREEN_BLE_MENU       11
#define SCREEN_SHIFT_SETTINGS 12

int currentScreen = SCREEN_ALL_IN_ONE;
int lastGaugeScreen = SCREEN_ALL_IN_ONE;

// =====================================================
// Ring Colors
// =====================================================

#define RING_CYAN      0
#define RING_BLUE      1
#define RING_RED       2
#define RING_YELLOW    3
#define RING_GREEN     4
#define RING_PURPLE    5
#define RING_MAGENTA   6
#define RING_SYNTHWAVE 7
#define RING_OFF       8

const int RING_COLOR_COUNT = 9;

int batteryRingColor = RING_CYAN;
int allInOneRingColor = RING_CYAN;
int rpmRingColor = RING_CYAN;
int speedRingColor = RING_CYAN;
int coolantRingColor = RING_CYAN;
int throttleRingColor = RING_CYAN;
int intakeAirRingColor = RING_CYAN;
int airflowRingColor = RING_CYAN;
int mpgRingColor = RING_CYAN;
int chronoRingColor = RING_BLUE;
int tpmsRingColor = RING_CYAN;
int oilTempRingColor = RING_CYAN;

// =====================================================
// Live OBD Values
// =====================================================

float liveBatteryVoltage = 0.0;
bool hasLiveVoltage = false;

const float BATTERY_MIN_VOLTAGE = 11.8;
const float BATTERY_MAX_VOLTAGE = 13.6;

int liveRpm = 0;
bool hasLiveRpm = false;

int liveSpeedKph = 0;
bool hasLiveSpeed = false;

int liveCoolantTempC = 0;
bool hasLiveCoolantTemp = false;

int liveThrottlePercent = 0;
bool hasLiveThrottle = false;

int liveIntakeAirTempC = 0;
bool hasLiveIntakeAirTemp = false;

float liveMafGps = 0.0;
bool hasLiveMaf = false;

int liveOilTempC = 0;
bool hasLiveOilTemp = false;

// =====================================================
// TPMS Values
// =====================================================
//
// UI first: actual Mazda TPMS OBD polling will be added after we confirm
// which extended PIDs your car/adapter returns.
float liveTpmsPsi[4] = {0.0, 0.0, 0.0, 0.0};
bool hasLiveTpms[4] = {false, false, false, false};

#define TPMS_UNIT_PSI 0
#define TPMS_UNIT_KPA 1

int tpmsUnitMode = TPMS_UNIT_PSI;

// =====================================================
// TPMS OBD Debug
// =====================================================
//
// v120 reset: All-In-One gauge removed for now while we plan the layout/polling strategy.
// Debug-only build: when the TPMS screen is open, Serial Monitor will print
// the raw Mazda extended PID responses so we can confirm whether the adapter
// and car expose tire pressures.
bool tpmsDebugHeaderSet = false;
int tpmsDebugStep = 0;
unsigned long lastTpmsDebugPollTime = 0;
const unsigned long TPMS_DEBUG_POLL_INTERVAL = 1500;

// =====================================================
// Chronograph / Tachymetre
// =====================================================

#define CHRONO_MODE_0_60    0
#define CHRONO_MODE_30_70   1
#define CHRONO_MODE_60_100  2
const int CHRONO_MODE_COUNT = 3;

int chronoMode = CHRONO_MODE_0_60;
bool chronoRunning = false;
unsigned long chronoStartMillis = 0;
unsigned long chronoElapsedMillis = 0;
unsigned long lastChronoRedrawTime = 0;
const unsigned long CHRONO_SECOND_HAND_TICK_MS = 1000; // 1 tick per second, like a classic stopwatch
const unsigned long CHRONO_REDRAW_INTERVAL = 20;        // check often, but redraw only when the visible tick changes
unsigned long lastChronoVisualTick = 0xFFFFFFFF;
int chronoPollStep = 0;

// Hardware button support for the watch-style chrono controls.
// BOOT is GPIO0 on ESP32-S3 boards and starts/stops the chronograph.
 // Screen tap resets the chronograph.
#define CHRONO_POWER_BUTTON_PIN -1    // PWR uses AXP2101 PMU, not a normal GPIO
#define CHRONO_BOOT_BUTTON_PIN   0    // start/stop using BOOT button

bool chronoPowerButtonLast = true;
bool chronoBootButtonLast = true;
unsigned long lastChronoButtonTime = 0;
const unsigned long CHRONO_BUTTON_DEBOUNCE = 180;

#if BLUELINE_HAS_XPOWERS
XPowersPMU chronoPower;
bool chronoPowerReady = false;
#endif

static lv_point_precise_t chronoLinePoints[120][2];
int chronoLinePointIndex = 0;

bool bleConnected = false;
bool obdReady = false;
bool bleStarted = false;

BLEClient *bleClient = nullptr;
BLERemoteCharacteristic *obdCharacteristic = nullptr;

BLEUUID OBD_SERVICE_UUID("0000ffe0-0000-1000-8000-00805f9b34fb");
BLEUUID OBD_CHAR_UUID("0000ffe1-0000-1000-8000-00805f9b34fb");

#define BLE_MAX_RESULTS 8

struct BleCandidate
{
  String name;
  String address;
  int rssi;
  int score;
};

BleCandidate bleResults[BLE_MAX_RESULTS];
int bleResultCount = 0;

String bleStatusText = "READY";
String connectedBleName = "";
String connectedBleAddress = "";

#define BLE_MENU_MAIN       0
#define BLE_MENU_SELECT     1
#define BLE_MENU_CONNECTING 2

int bleMenuState = BLE_MENU_MAIN;
String bleConnectMessage = "READY";
String bleConnectingName = "";

// =====================================================
// BLE Auto Reconnect
// =====================================================

bool autoReconnectEnabled = false; // boot screen handles the one visible auto reconnect attempt
bool autoReconnectInProgress = false;
int autoReconnectAttempts = 0;

unsigned long bootCompleteTime = 0;
unsigned long lastAutoReconnectAttemptTime = 0;

const int AUTO_RECONNECT_MAX_ATTEMPTS = 1;
const unsigned long AUTO_RECONNECT_START_DELAY = 1000;
const unsigned long AUTO_RECONNECT_RETRY_DELAY = 1500;

// =====================================================
// OBD polling
// =====================================================

String obdRxBuffer = "";
bool obdNotifyReady = false;
bool obdInitDone = false;

unsigned long lastObdPollTime = 0;
const unsigned long OBD_POLL_INTERVAL = 120;

// RPM Turbo Mode: when the RPM gauge is active, poll only RPM faster.
// OBD over BLE still has hardware limits, but this reduces intentional wait time.
#define RPM_TURBO_MODE true
const unsigned long RPM_TURBO_INTERVAL = 50;
const unsigned long RPM_TURBO_TIMEOUT = 55;

#define OBD_POLL_VOLTAGE  0
#define OBD_POLL_RPM      1
#define OBD_POLL_SPEED    2
#define OBD_POLL_COOLANT  3
#define OBD_POLL_THROTTLE 4
#define OBD_POLL_INTAKE_AIR 5
#define OBD_POLL_AIRFLOW    6
#define OBD_POLL_OIL_TEMP   7

int obdPollStep = OBD_POLL_VOLTAGE;
int mpgPollStep = 0; // 0 = speed, 1 = MAF for Average MPG screen

// All-In-One RPM-priority batch/turbo style polling:
// 0 = RPM, 1 = Speed, 2 = RPM, 3 = Throttle, 4 = RPM, 5 = Intake Air
int allInOnePollStep = 0;

// =====================================================
// Average MPG
// =====================================================
//
// Average MPG uses trip distance / estimated fuel used.
// Distance comes from speed. Fuel estimate comes from MAF.
// This is stored as trip miles and trip gallons instead of
// averaging instant MPG values, which is more stable.
float mpgTripMiles = 0.0;
float mpgTripGallons = 0.0;

unsigned long lastMpgAverageUpdateTime = 0;
unsigned long lastMpgAverageSaveTime = 0;

const unsigned long MPG_AVERAGE_UPDATE_INTERVAL = 1000;
const unsigned long MPG_AVERAGE_SAVE_INTERVAL = 60000;

// =====================================================
// Speed / Temperature Units
// =====================================================

#define SPEED_UNIT_MPH 0
#define SPEED_UNIT_KPH 1

int speedUnitMode = SPEED_UNIT_MPH;

#define TEMP_UNIT_F 0
#define TEMP_UNIT_C 1

int coolantUnitMode = TEMP_UNIT_F;
int intakeAirUnitMode = TEMP_UNIT_F;
int oilTempUnitMode = TEMP_UNIT_F;

// =====================================================
// Brightness Settings
// =====================================================

#define BRIGHTNESS_DIM    0
#define BRIGHTNESS_MEDIUM 1
#define BRIGHTNESS_BRIGHT 2

int brightnessMode = BRIGHTNESS_BRIGHT;

const unsigned long DOUBLE_TAP_TIME = 400;
const int DOUBLE_TAP_DISTANCE = 90;

unsigned long lastTapTime = 0;
int lastTapX = 0;
int lastTapY = 0;

// =====================================================
// Shift Light Settings
// =====================================================

const int SHIFT_STEP_COUNT = 10;

int selectedShiftStep = 0;

int shiftLightRpm[SHIFT_STEP_COUNT] = {
  750, 1500, 2250, 3000,
  3750, 4500, 5250,
  6000, 6750,
  7500
};

const int SHIFT_RPM_MIN = 500;
const int SHIFT_RPM_MAX = 9000;
const int SHIFT_RPM_STEP = 25;

// =====================================================
// Forward Declarations
// =====================================================

void redrawUI();
void drawRings();
void drawCurrentScreen();

void showBatteryGauge();
void showAllInOneGauge();
void showRpmGauge();
void showSpeedGauge();
void showCoolantGauge();
void showThrottleGauge();
void showIntakeAirGauge();
void showAirflowGauge();
void showMpgGauge();
void showChronographGauge();
void showTpmsGauge();
void showOilTempGauge();
void showBleMenu();
void showShiftSettings();

void nextGauge();
void previousGauge();

void nextRingColor();
void previousRingColor();

void setupTouch();
void handleTouch();
bool readTouchPoint(int &x, int &y);
void handleTap(int x, int y);
void handleSwipe(int dx, int dy, unsigned long duration);

int getLongPressTargetFromStart(int x, int y);
void triggerLongPressTarget();

void handleRepeatButtonHold();
void adjustSelectedShiftRpm(int amount);
void setupChronographButtons();
void handleChronographButtons();
void toggleChronograph();
void resetChronograph();
void updateChronograph();
unsigned long getChronographElapsedMs();
const char *getChronographModeText();
const char *getChronographStatusText();
void nextChronographMode();
void previousChronographMode();
float getLiveSpeedMph();
float getCurrentMpgEstimate(bool &hasMpg);
bool calculateAverageMpg(float &mpgOut);
void updateAverageMpg();
void resetAverageMpg();
void saveMpgAverageSettings();

void loadSettings();
void saveRingSettings();
void saveSpeedSettings();
void saveCoolantUnitSettings();
void saveIntakeAirUnitSettings();
void saveOilTempUnitSettings();
void saveTpmsUnitSettings();
void loadShiftLightSettings();
void saveShiftLightSettings();

void openBleMenu();
void goBackToLastGauge();
void drawBluetoothIcon();
void drawGearIcon();
void drawAllInOneThrottleBars(int percent, bool hasData, uint32_t activeColor);
void drawAllInOneDivider();
void drawCoolantIcon(uint32_t iconColor);
void drawIntakeAirIcon(uint32_t iconColor);
void drawAirflowIcon(uint32_t iconColor);
void drawChronoDial(uint32_t mainColor, uint32_t highlightColor);
void drawChronoSubdialNumber(int cx, int cy, const char *text, float angleDeg, int radius, uint32_t color);
void drawChronoSubdial(int cx, int cy, int radius, const char *topText, const char *rightLowerText, const char *leftLowerText, uint32_t color, float handAngleDeg);
void drawChronoHand(float angleDeg, int length, uint32_t color, int width);
void drawTpmsTire(int x, int y, const char *cornerLabel, float psi, bool hasData, uint32_t color);
uint32_t getTpmsColor(float psi, bool hasData);
uint32_t getTpmsStatusLineColor(float psi, bool hasData);
void saveTpmsUnitSettings();
void drawOilTempIcon(uint32_t iconColor);
void drawThrottleSegments(int percent, bool hasData, uint32_t activeColor);
void drawBootAnimation();
void showBootReconnectStatus(const char *statusText, uint32_t statusColor);
void runBootAutoReconnect();

void initBleIfNeeded();
void forceLvglRefresh();
void scanBleDevices();
int scoreBleDevice(BLEAdvertisedDevice device);
void sortBleResults();
bool connectToBleResult(int index, int failReturnState = BLE_MENU_SELECT);
bool connectToSavedBle();
bool connectToSavedBleQuiet();
void handleAutoReconnect();
void disconnectBle();
String shortenBleName(String name, int maxLen);
void clearLiveData();

int getBrightnessValue();
void applyBrightness();
void cycleBrightness();
bool isGaugeScreen();
bool checkDoubleTap(int x, int y);
void saveBrightnessSettings();

void onObdNotify(BLERemoteCharacteristic *characteristic, uint8_t *data, size_t length, bool isNotify);
void clearObdRxBuffer();
bool sendObdCommandRaw(const String &command);
String sendObdCommand(const String &command, unsigned long timeoutMs = 350);
bool initializeObdAdapter();
String getCurrentGaugeObdCommand();
unsigned long getCurrentGaugePollInterval(const String &command);
unsigned long getCurrentGaugeObdTimeout(const String &command);
void pollTpmsDebug()
{
  if (!bleConnected || !obdReady || obdCharacteristic == nullptr)
  {
    return;
  }

  if (currentScreen != SCREEN_TPMS)
  {
    if (tpmsDebugHeaderSet)
    {
      Serial.println("[TPMS DEBUG] Leaving TPMS screen, restoring functional OBD header AT SH 7DF");
      sendObdCommand("AT SH 7DF", 350);
    }

    tpmsDebugHeaderSet = false;
    tpmsDebugStep = 0;
    return;
  }

  unsigned long now = millis();

  if (now - lastTpmsDebugPollTime < TPMS_DEBUG_POLL_INTERVAL)
  {
    return;
  }

  lastTpmsDebugPollTime = now;

  if (!tpmsDebugHeaderSet)
  {
    Serial.println();
    Serial.println("[TPMS DEBUG] Setting Mazda TPMS header: AT SH 720");
    String headerResponse = sendObdCommand("AT SH 720", 700);
    Serial.print("[TPMS DEBUG] AT SH 720 response: ");
    Serial.println(headerResponse);

    tpmsDebugHeaderSet = true;
    tpmsDebugStep = 0;
    return;
  }

  const char *labels[] = {"FL", "FR", "RL", "RR"};
  const char *commands[] = {"222A05", "222A07", "222A06", "222A08"};

  const char *label = labels[tpmsDebugStep];
  const char *command = commands[tpmsDebugStep];

  Serial.print("[TPMS DEBUG] ");
  Serial.print(label);
  Serial.print(" request ");
  Serial.print(command);
  Serial.print(" -> ");

  String response = sendObdCommand(String(command), 900);
  Serial.println(response);

  tpmsDebugStep++;

  if (tpmsDebugStep >= 4)
  {
    tpmsDebugStep = 0;
    Serial.println("[TPMS DEBUG] Cycle complete. Looking for responses like: 62 2A 05 ...");
  }
}

void pollObdLiveData();
void parseObdResponse(const String &command, const String &response);
void parseBatteryVoltage(const String &response);
void parseControlModuleVoltage(const String &response);
void parseRpm(const String &response);
void parseSpeed(const String &response);
void parseCoolantTemp(const String &response);
void parseThrottlePosition(const String &response);
void parseIntakeAirTemp(const String &response);
void parseMafAirflow(const String &response);
void parseOilTemp(const String &response);
void pollTpmsDebug();
int hexByteToInt(const String &token);

lv_obj_t *createButton(
  lv_obj_t *parent,
  const char *text,
  int x,
  int y,
  int w,
  int h,
  uint32_t borderColor,
  uint32_t textColor
);

lv_obj_t *createBigButton(
  lv_obj_t *parent,
  const char *text,
  int x,
  int y,
  int w,
  int h,
  uint32_t borderColor,
  uint32_t textColor
);

lv_obj_t *createCircleMenuButton(
  lv_obj_t *parent,
  const char *symbol,
  const char *labelText,
  int cx,
  int cy,
  uint32_t borderColor,
  uint32_t symbolColor,
  uint32_t labelColor
);

lv_obj_t *createCircleButtonCustom(
  lv_obj_t *parent,
  const char *symbol,
  const char *labelText,
  int cx,
  int cy,
  int circleSize,
  int labelOffsetY,
  const lv_font_t *symbolFont,
  const lv_font_t *labelFont,
  uint32_t borderColor,
  uint32_t symbolColor,
  uint32_t labelColor
);

int wrapRingColor(int colorMode);
int getCurrentRingColor();
void setCurrentRingColor(int colorMode);

uint32_t getRingGlowColor(int mode);
uint32_t getRingMainColor(int mode);
uint32_t getRingHighlightColor(int mode);
uint32_t getRingInnerColor(int mode);

// =====================================================
// LVGL Flush
// =====================================================

void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map)
{
  int32_t w = lv_area_get_width(area);
  int32_t h = lv_area_get_height(area);

  gfx->draw16bitRGBBitmap(
    area->x1,
    area->y1,
    (uint16_t *)px_map,
    w,
    h
  );

  lv_display_flush_ready(disp);
}

// =====================================================
// Utility
// =====================================================

bool isInRect(int touchX, int touchY, int x, int y, int w, int h)
{
  return touchX >= x &&
         touchX <= x + w &&
         touchY >= y &&
         touchY <= y + h;
}

void clearLayer(lv_obj_t *layer)
{
  if (layer != nullptr)
  {
    lv_obj_clean(layer);
  }
}

lv_obj_t *createSolidRect(
  lv_obj_t *parent,
  int w,
  int h,
  int x,
  int y,
  uint32_t color,
  lv_opa_t opacity,
  int radius
)
{
  lv_obj_t *obj = lv_obj_create(parent);
  lv_obj_set_size(obj, w, h);
  lv_obj_align(obj, LV_ALIGN_CENTER, x, y);
  lv_obj_set_style_bg_color(obj, lv_color_hex(color), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(obj, opacity, LV_PART_MAIN);
  lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(obj, radius, LV_PART_MAIN);
  lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
  return obj;
}

// =====================================================
// LVGL Helpers
// =====================================================

lv_obj_t *createRing(
  lv_obj_t *parent,
  int size,
  int borderWidth,
  uint32_t color,
  lv_opa_t opacity
)
{
  lv_obj_t *ring = lv_obj_create(parent);

  lv_obj_set_size(ring, size, size);
  lv_obj_center(ring);

  lv_obj_set_style_bg_opa(ring, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_color(ring, lv_color_hex(color), LV_PART_MAIN);
  lv_obj_set_style_border_width(ring, borderWidth, LV_PART_MAIN);
  lv_obj_set_style_border_opa(ring, opacity, LV_PART_MAIN);
  lv_obj_set_style_radius(ring, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_pad_all(ring, 0, LV_PART_MAIN);
  lv_obj_set_style_outline_width(ring, 0, LV_PART_MAIN);
  lv_obj_set_style_shadow_width(ring, 0, LV_PART_MAIN);

  lv_obj_clear_flag(ring, LV_OBJ_FLAG_SCROLLABLE);

  return ring;
}

lv_obj_t *createLabel(
  lv_obj_t *parent,
  const char *text,
  const lv_font_t *font,
  uint32_t color,
  int x,
  int y
)
{
  lv_obj_t *label = lv_label_create(parent);

  lv_label_set_text(label, text);
  lv_obj_set_style_text_font(label, font, LV_PART_MAIN);
  lv_obj_set_style_text_color(label, lv_color_hex(color), LV_PART_MAIN);
  lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(label, LV_ALIGN_CENTER, x, y);

  return label;
}

lv_obj_t *createButton(
  lv_obj_t *parent,
  const char *text,
  int x,
  int y,
  int w,
  int h,
  uint32_t borderColor,
  uint32_t textColor
)
{
  lv_obj_t *btn = lv_obj_create(parent);

  lv_obj_set_size(btn, w, h);
  lv_obj_align(btn, LV_ALIGN_TOP_LEFT, x, y);

  lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_color(btn, lv_color_hex(borderColor), LV_PART_MAIN);
  lv_obj_set_style_border_width(btn, 2, LV_PART_MAIN);
  lv_obj_set_style_radius(btn, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_all(btn, 0, LV_PART_MAIN);

  lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, text);
  lv_obj_set_style_text_font(label, &Xolonium_pn4D18pt7b, LV_PART_MAIN);
  lv_obj_set_style_text_color(label, lv_color_hex(textColor), LV_PART_MAIN);

  lv_obj_center(label);

  return btn;
}

lv_obj_t *createBigButton(
  lv_obj_t *parent,
  const char *text,
  int x,
  int y,
  int w,
  int h,
  uint32_t borderColor,
  uint32_t textColor
)
{
  lv_obj_t *btn = lv_obj_create(parent);

  lv_obj_set_size(btn, w, h);
  lv_obj_align(btn, LV_ALIGN_TOP_LEFT, x, y);

  lv_obj_set_style_bg_opa(btn, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_color(btn, lv_color_hex(borderColor), LV_PART_MAIN);
  lv_obj_set_style_border_width(btn, 2, LV_PART_MAIN);
  lv_obj_set_style_radius(btn, 10, LV_PART_MAIN);
  lv_obj_set_style_pad_all(btn, 0, LV_PART_MAIN);

  lv_obj_clear_flag(btn, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *label = lv_label_create(btn);
  lv_label_set_text(label, text);

  // Bigger + / - text
  lv_obj_set_style_text_font(label, &Xolonium_pn4D32pt7b, LV_PART_MAIN);
  lv_obj_set_style_text_color(label, lv_color_hex(textColor), LV_PART_MAIN);

  lv_obj_center(label);

  return btn;
}


lv_obj_t *createCircleMenuButton(
  lv_obj_t *parent,
  const char *symbol,
  const char *labelText,
  int cx,
  int cy,
  uint32_t borderColor,
  uint32_t symbolColor,
  uint32_t labelColor
)
{
  const int circleSize = 76;

  int offsetX = cx - (LCD_WIDTH / 2);
  int offsetY = cy - (LCD_HEIGHT / 2);

  // Circle tap target
  lv_obj_t *circle = lv_obj_create(parent);
  lv_obj_set_size(circle, circleSize, circleSize);
  lv_obj_align(circle, LV_ALIGN_CENTER, offsetX, offsetY);

  lv_obj_set_style_bg_opa(circle, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_color(circle, lv_color_hex(borderColor), LV_PART_MAIN);
  lv_obj_set_style_border_width(circle, 3, LV_PART_MAIN);
  lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_pad_all(circle, 0, LV_PART_MAIN);
  lv_obj_clear_flag(circle, LV_OBJ_FLAG_SCROLLABLE);

  // Soft inner glow
  lv_obj_t *glow = lv_obj_create(parent);
  lv_obj_set_size(glow, circleSize - 12, circleSize - 12);
  lv_obj_align(glow, LV_ALIGN_CENTER, offsetX, offsetY);
  lv_obj_set_style_bg_color(glow, lv_color_hex(borderColor), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(glow, LV_OPA_20, LV_PART_MAIN);
  lv_obj_set_style_border_width(glow, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(glow, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_pad_all(glow, 0, LV_PART_MAIN);
  lv_obj_clear_flag(glow, LV_OBJ_FLAG_SCROLLABLE);

  // Keep the border above the glow
  lv_obj_move_foreground(circle);

  lv_obj_t *symbolLabel = lv_label_create(circle);
  lv_label_set_text(symbolLabel, symbol);
  lv_obj_set_style_text_font(symbolLabel, &Xolonium_pn4D24pt7b, LV_PART_MAIN);
  lv_obj_set_style_text_color(symbolLabel, lv_color_hex(symbolColor), LV_PART_MAIN);
  lv_obj_center(symbolLabel);

  lv_obj_t *textLabel = lv_label_create(parent);
  lv_label_set_text(textLabel, labelText);

  // Keep all BLE menu labels the same size, including RECONNECT
  lv_obj_set_style_text_font(textLabel, &Xolonium_pn4D18pt7b, LV_PART_MAIN);

  lv_obj_set_style_text_color(textLabel, lv_color_hex(labelColor), LV_PART_MAIN);
  lv_obj_set_style_text_align(textLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(textLabel, LV_ALIGN_CENTER, offsetX, offsetY + 56);

  return circle;
}

lv_obj_t *createCircleButtonCustom(
  lv_obj_t *parent,
  const char *symbol,
  const char *labelText,
  int cx,
  int cy,
  int circleSize,
  int labelOffsetY,
  const lv_font_t *symbolFont,
  const lv_font_t *labelFont,
  uint32_t borderColor,
  uint32_t symbolColor,
  uint32_t labelColor
)
{
  int offsetX = cx - (LCD_WIDTH / 2);
  int offsetY = cy - (LCD_HEIGHT / 2);

  lv_obj_t *circle = lv_obj_create(parent);
  lv_obj_set_size(circle, circleSize, circleSize);
  lv_obj_align(circle, LV_ALIGN_CENTER, offsetX, offsetY);

  lv_obj_set_style_bg_opa(circle, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_color(circle, lv_color_hex(borderColor), LV_PART_MAIN);
  lv_obj_set_style_border_width(circle, 3, LV_PART_MAIN);
  lv_obj_set_style_radius(circle, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_pad_all(circle, 0, LV_PART_MAIN);
  lv_obj_clear_flag(circle, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *glow = lv_obj_create(parent);
  lv_obj_set_size(glow, circleSize - 12, circleSize - 12);
  lv_obj_align(glow, LV_ALIGN_CENTER, offsetX, offsetY);
  lv_obj_set_style_bg_color(glow, lv_color_hex(borderColor), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(glow, LV_OPA_20, LV_PART_MAIN);
  lv_obj_set_style_border_width(glow, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(glow, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_pad_all(glow, 0, LV_PART_MAIN);
  lv_obj_clear_flag(glow, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_move_foreground(circle);

  lv_obj_t *symbolLabel = lv_label_create(circle);
  lv_label_set_text(symbolLabel, symbol);
  lv_obj_set_style_text_font(symbolLabel, symbolFont, LV_PART_MAIN);
  lv_obj_set_style_text_color(symbolLabel, lv_color_hex(symbolColor), LV_PART_MAIN);
  lv_obj_center(symbolLabel);

  lv_obj_t *textLabel = lv_label_create(parent);
  lv_label_set_text(textLabel, labelText);
  lv_obj_set_style_text_font(textLabel, labelFont, LV_PART_MAIN);
  lv_obj_set_style_text_color(textLabel, lv_color_hex(labelColor), LV_PART_MAIN);
  lv_obj_set_style_text_align(textLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(textLabel, LV_ALIGN_CENTER, offsetX, offsetY + labelOffsetY);

  return circle;
}

lv_obj_t *createDot(
  lv_obj_t *parent,
  int x,
  int y,
  int size,
  uint32_t color,
  lv_opa_t opacity
)
{
  lv_obj_t *dot = lv_obj_create(parent);

  lv_obj_set_size(dot, size, size);
  lv_obj_align(dot, LV_ALIGN_CENTER, x, y);

  lv_obj_set_style_bg_color(dot, lv_color_hex(color), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(dot, opacity, LV_PART_MAIN);
  lv_obj_set_style_border_width(dot, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_pad_all(dot, 0, LV_PART_MAIN);

  lv_obj_clear_flag(dot, LV_OBJ_FLAG_SCROLLABLE);

  return dot;
}

void createChronoLine(int x1, int y1, int x2, int y2, uint32_t color, int width, lv_opa_t opacity)
{
  if (chronoLinePointIndex >= 120)
  {
    return;
  }

  int i = chronoLinePointIndex++;
  chronoLinePoints[i][0].x = 233 + x1;
  chronoLinePoints[i][0].y = 233 + y1;
  chronoLinePoints[i][1].x = 233 + x2;
  chronoLinePoints[i][1].y = 233 + y2;

  lv_obj_t *line = lv_line_create(contentLayer);
  lv_obj_set_size(line, LCD_WIDTH, LCD_HEIGHT);
  lv_obj_center(line);
  lv_line_set_points(line, chronoLinePoints[i], 2);
  lv_obj_set_style_line_color(line, lv_color_hex(color), LV_PART_MAIN);
  lv_obj_set_style_line_width(line, width, LV_PART_MAIN);
  lv_obj_set_style_line_opa(line, opacity, LV_PART_MAIN);
  lv_obj_set_style_line_rounded(line, true, LV_PART_MAIN);
  lv_obj_clear_flag(line, LV_OBJ_FLAG_SCROLLABLE);
}

void createChronoTick(float angleDeg, int innerRadius, int outerRadius, uint32_t color, int width, lv_opa_t opacity)
{
  float rad = (angleDeg - 90.0) * DEG_TO_RAD;
  int x1 = (int)(cos(rad) * innerRadius);
  int y1 = (int)(sin(rad) * innerRadius);
  int x2 = (int)(cos(rad) * outerRadius);
  int y2 = (int)(sin(rad) * outerRadius);

  createChronoLine(x1, y1, x2, y2, color, width, opacity);
}

void createChronoLabel(const char *text, const lv_font_t *font, uint32_t color, float angleDeg, int radius)
{
  float rad = (angleDeg - 90.0) * DEG_TO_RAD;
  int x = (int)(cos(rad) * radius);
  int y = (int)(sin(rad) * radius);
  createLabel(contentLayer, text, font, color, x, y);
}

float getTachymetreAngle(int value)
{
  // Tachymetre scale is not evenly spaced.
  // value = 3600 / seconds, so seconds = 3600 / value.
  // On a 60-second dial, each second is 6 degrees.
  float seconds = 3600.0 / (float)value;
  float angle = seconds * 6.0;

  while (angle >= 360.0) angle -= 360.0;
  while (angle < 0.0) angle += 360.0;

  return angle;
}

void createTachymetreLabel(const char *text, int value, const lv_font_t *font, uint32_t color, int radius)
{
  createChronoLabel(text, font, color, getTachymetreAngle(value), radius);
}

void createTachymetreMarker(int value, int innerRadius, int outerRadius, uint32_t color, int width, lv_opa_t opacity)
{
  createChronoTick(getTachymetreAngle(value), innerRadius, outerRadius, color, width, opacity);
}

// =====================================================
// Bluetooth / Gear Icons
// =====================================================

void openBleMenu()
{
  if (currentScreen == SCREEN_BATTERY ||
      currentScreen == SCREEN_ALL_IN_ONE ||
      currentScreen == SCREEN_RPM ||
      currentScreen == SCREEN_SPEED ||
      currentScreen == SCREEN_COOLANT ||
      currentScreen == SCREEN_THROTTLE ||
      currentScreen == SCREEN_INTAKE_AIR ||
      currentScreen == SCREEN_AIRFLOW ||
      currentScreen == SCREEN_MPG ||
      currentScreen == SCREEN_CHRONO ||
      currentScreen == SCREEN_TPMS ||
      currentScreen == SCREEN_OIL_TEMP)
  {
    lastGaugeScreen = currentScreen;
  }

  currentScreen = SCREEN_BLE_MENU;
  redrawUI();
}

void goBackToLastGauge()
{
  if (lastGaugeScreen != SCREEN_BATTERY &&
      lastGaugeScreen != SCREEN_ALL_IN_ONE &&
      lastGaugeScreen != SCREEN_RPM &&
      lastGaugeScreen != SCREEN_SPEED &&
      lastGaugeScreen != SCREEN_COOLANT &&
      lastGaugeScreen != SCREEN_THROTTLE &&
      lastGaugeScreen != SCREEN_INTAKE_AIR &&
      lastGaugeScreen != SCREEN_AIRFLOW &&
      lastGaugeScreen != SCREEN_MPG &&
      lastGaugeScreen != SCREEN_CHRONO &&
      lastGaugeScreen != SCREEN_TPMS &&
      lastGaugeScreen != SCREEN_OIL_TEMP)
  {
    lastGaugeScreen = SCREEN_BATTERY;
  }

  currentScreen = lastGaugeScreen;
  redrawUI();
}

void drawBluetoothIcon()
{
  uint32_t iconColor = bleConnected ? 0x0095FF : 0x7A7A7A;

  lv_obj_t *btLabel = lv_label_create(contentLayer);

  lv_label_set_text(btLabel, LV_SYMBOL_BLUETOOTH);
  lv_obj_set_style_text_color(btLabel, lv_color_hex(iconColor), LV_PART_MAIN);
  lv_obj_set_style_text_font(btLabel, &lv_font_montserrat_40, LV_PART_MAIN);

  lv_obj_align(btLabel, LV_ALIGN_CENTER, 0, 170);
}

void drawGearIcon()
{
  uint32_t iconColor = getRingHighlightColor(getCurrentRingColor());

  lv_obj_t *gearLabel = lv_label_create(contentLayer);

  lv_label_set_text(gearLabel, LV_SYMBOL_SETTINGS);
  lv_obj_set_style_text_color(gearLabel, lv_color_hex(iconColor), LV_PART_MAIN);
  lv_obj_set_style_text_font(gearLabel, &lv_font_montserrat_40, LV_PART_MAIN);

  lv_obj_align(gearLabel, LV_ALIGN_CENTER, 0, 75);
}

void drawCoolantIcon(uint32_t iconColor)
{
  // Simple dashboard-style coolant thermometer icon.
  // Uses LVGL shapes instead of emoji so it renders consistently.

  const int stemX = 0;
  const int stemY = 66;
  const int bulbY = 100;

  // Soft glow behind the thermometer bulb.
  createDot(contentLayer, stemX, bulbY, 38, iconColor, LV_OPA_20);

  // Thermometer stem and bulb.
  createSolidRect(contentLayer, 10, 58, stemX, stemY, iconColor, LV_OPA_COVER, 5);
  createDot(contentLayer, stemX, bulbY, 28, iconColor, LV_OPA_COVER);

  // Thermometer tick marks.
  createSolidRect(contentLayer, 17, 4, stemX + 14, stemY - 18, iconColor, LV_OPA_COVER, 2);
  createSolidRect(contentLayer, 14, 4, stemX + 12, stemY - 5, iconColor, LV_OPA_COVER, 2);
  createSolidRect(contentLayer, 17, 4, stemX + 14, stemY + 8, iconColor, LV_OPA_COVER, 2);

  // Coolant wave lines below the bulb.
  createSolidRect(contentLayer, 20, 4, -24, 123, iconColor, LV_OPA_COVER, 2);
  createSolidRect(contentLayer, 24, 4, 0, 123, iconColor, LV_OPA_COVER, 2);
  createSolidRect(contentLayer, 20, 4, 24, 123, iconColor, LV_OPA_COVER, 2);

  createSolidRect(contentLayer, 20, 4, -14, 134, iconColor, LV_OPA_80, 2);
  createSolidRect(contentLayer, 24, 4, 10, 134, iconColor, LV_OPA_80, 2);
}

void drawIntakeAirIcon(uint32_t iconColor)
{
  // Simple intake-air icon drawn with LVGL shapes:
  // filter box on the left, airflow lines and dots moving right.

  const int iconY = 82;

  // Soft glow behind the intake icon.
  createDot(contentLayer, 0, iconY, 96, iconColor, LV_OPA_10);

  // Filter box outline.
  lv_obj_t *filterBox = lv_obj_create(contentLayer);
  lv_obj_set_size(filterBox, 64, 56);
  lv_obj_align(filterBox, LV_ALIGN_CENTER, -52, iconY);
  lv_obj_set_style_bg_opa(filterBox, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_color(filterBox, lv_color_hex(iconColor), LV_PART_MAIN);
  lv_obj_set_style_border_width(filterBox, 4, LV_PART_MAIN);
  lv_obj_set_style_radius(filterBox, 6, LV_PART_MAIN);
  lv_obj_set_style_pad_all(filterBox, 0, LV_PART_MAIN);
  lv_obj_clear_flag(filterBox, LV_OBJ_FLAG_SCROLLABLE);

  // Filter ribs.
  createSolidRect(contentLayer, 5, 38, -70, iconY, iconColor, LV_OPA_COVER, 2);
  createSolidRect(contentLayer, 5, 38, -58, iconY, iconColor, LV_OPA_COVER, 2);
  createSolidRect(contentLayer, 5, 38, -46, iconY, iconColor, LV_OPA_COVER, 2);
  createSolidRect(contentLayer, 5, 38, -34, iconY, iconColor, LV_OPA_COVER, 2);

  // Intake neck / airflow path.
  createSolidRect(contentLayer, 34, 7, -6, iconY - 17, iconColor, LV_OPA_COVER, 4);
  createSolidRect(contentLayer, 34, 7, -6, iconY + 17, iconColor, LV_OPA_COVER, 4);

  // Airflow lines.
  createSolidRect(contentLayer, 42, 5, 42, iconY - 20, iconColor, LV_OPA_COVER, 3);
  createSolidRect(contentLayer, 34, 5, 48, iconY, iconColor, LV_OPA_COVER, 3);
  createSolidRect(contentLayer, 42, 5, 42, iconY + 20, iconColor, LV_OPA_COVER, 3);

  // Air dots.
  createDot(contentLayer, 78, iconY - 20, 9, iconColor, LV_OPA_COVER);
  createDot(contentLayer, 86, iconY, 9, iconColor, LV_OPA_90);
  createDot(contentLayer, 78, iconY + 20, 9, iconColor, LV_OPA_80);
}

// =====================================================
// Ring Color Helpers
// =====================================================

int wrapRingColor(int colorMode)
{
  if (colorMode < 0)
  {
    return RING_COLOR_COUNT - 1;
  }

  if (colorMode >= RING_COLOR_COUNT)
  {
    return 0;
  }

  return colorMode;
}

int getCurrentRingColor()
{
  if (currentScreen == SCREEN_BATTERY)
  {
    return batteryRingColor;
  }

  if (currentScreen == SCREEN_ALL_IN_ONE)
  {
    return allInOneRingColor;
  }

  if (currentScreen == SCREEN_RPM || currentScreen == SCREEN_SHIFT_SETTINGS)
  {
    return rpmRingColor;
  }

  if (currentScreen == SCREEN_SPEED)
  {
    return speedRingColor;
  }

  if (currentScreen == SCREEN_COOLANT)
  {
    return coolantRingColor;
  }

  if (currentScreen == SCREEN_THROTTLE)
  {
    return throttleRingColor;
  }

  if (currentScreen == SCREEN_INTAKE_AIR)
  {
    return intakeAirRingColor;
  }

  if (currentScreen == SCREEN_AIRFLOW)
  {
    return airflowRingColor;
  }

  if (currentScreen == SCREEN_MPG)
  {
    return mpgRingColor;
  }

  if (currentScreen == SCREEN_CHRONO)
  {
    return chronoRingColor;
  }

  if (currentScreen == SCREEN_TPMS)
  {
    return tpmsRingColor;
  }

  if (currentScreen == SCREEN_OIL_TEMP)
  {
    return oilTempRingColor;
  }

  if (currentScreen == SCREEN_BLE_MENU)
  {
    if (lastGaugeScreen == SCREEN_ALL_IN_ONE)
    {
      return allInOneRingColor;
    }

    if (lastGaugeScreen == SCREEN_RPM)
    {
      return rpmRingColor;
    }

    if (lastGaugeScreen == SCREEN_SPEED)
    {
      return speedRingColor;
    }

    if (lastGaugeScreen == SCREEN_COOLANT)
    {
      return coolantRingColor;
    }

    if (lastGaugeScreen == SCREEN_THROTTLE)
    {
      return throttleRingColor;
    }

    if (lastGaugeScreen == SCREEN_INTAKE_AIR)
    {
      return intakeAirRingColor;
    }

    if (lastGaugeScreen == SCREEN_AIRFLOW)
    {
      return airflowRingColor;
    }

    if (lastGaugeScreen == SCREEN_MPG)
    {
      return mpgRingColor;
    }

    if (lastGaugeScreen == SCREEN_CHRONO)
    {
      return chronoRingColor;
    }

    if (lastGaugeScreen == SCREEN_TPMS)
    {
      return tpmsRingColor;
    }

    if (lastGaugeScreen == SCREEN_OIL_TEMP)
    {
      return oilTempRingColor;
    }

    return batteryRingColor;
  }

  return batteryRingColor;
}

void setCurrentRingColor(int colorMode)
{
  colorMode = wrapRingColor(colorMode);

  if (currentScreen == SCREEN_BATTERY)
  {
    batteryRingColor = colorMode;
  }
  else if (currentScreen == SCREEN_ALL_IN_ONE)
  {
    allInOneRingColor = colorMode;
  }
  else if (currentScreen == SCREEN_RPM || currentScreen == SCREEN_SHIFT_SETTINGS)
  {
    rpmRingColor = colorMode;
  }
  else if (currentScreen == SCREEN_SPEED)
  {
    speedRingColor = colorMode;
  }
  else if (currentScreen == SCREEN_COOLANT)
  {
    coolantRingColor = colorMode;
  }
  else if (currentScreen == SCREEN_THROTTLE)
  {
    throttleRingColor = colorMode;
  }
  else if (currentScreen == SCREEN_INTAKE_AIR)
  {
    intakeAirRingColor = colorMode;
  }
  else if (currentScreen == SCREEN_AIRFLOW)
  {
    airflowRingColor = colorMode;
  }
  else if (currentScreen == SCREEN_MPG)
  {
    mpgRingColor = colorMode;
  }
  else if (currentScreen == SCREEN_CHRONO)
  {
    chronoRingColor = colorMode;
  }
  else if (currentScreen == SCREEN_TPMS)
  {
    tpmsRingColor = colorMode;
  }
  else if (currentScreen == SCREEN_OIL_TEMP)
  {
    oilTempRingColor = colorMode;
  }
  else if (currentScreen == SCREEN_BLE_MENU)
  {
    if (lastGaugeScreen == SCREEN_ALL_IN_ONE)
    {
      allInOneRingColor = colorMode;
    }
    else if (lastGaugeScreen == SCREEN_RPM)
    {
      rpmRingColor = colorMode;
    }
    else if (lastGaugeScreen == SCREEN_SPEED)
    {
      speedRingColor = colorMode;
    }
    else if (lastGaugeScreen == SCREEN_COOLANT)
    {
      coolantRingColor = colorMode;
    }
    else if (lastGaugeScreen == SCREEN_THROTTLE)
    {
      throttleRingColor = colorMode;
    }
    else if (lastGaugeScreen == SCREEN_INTAKE_AIR)
    {
      intakeAirRingColor = colorMode;
    }
    else if (lastGaugeScreen == SCREEN_AIRFLOW)
    {
      airflowRingColor = colorMode;
    }
    else if (lastGaugeScreen == SCREEN_MPG)
    {
      mpgRingColor = colorMode;
    }
    else if (lastGaugeScreen == SCREEN_CHRONO)
    {
      chronoRingColor = colorMode;
    }
    else if (lastGaugeScreen == SCREEN_TPMS)
    {
      tpmsRingColor = colorMode;
    }
    else if (lastGaugeScreen == SCREEN_OIL_TEMP)
    {
      oilTempRingColor = colorMode;
    }
    else
    {
      batteryRingColor = colorMode;
    }
  }

  saveRingSettings();
}

uint32_t getRingGlowColor(int mode)
{
  switch (wrapRingColor(mode))
  {
    case RING_BLUE:      return 0x001040;
    case RING_RED:       return 0x400000;
    case RING_YELLOW:    return 0x403800;
    case RING_GREEN:     return 0x003000;
    case RING_PURPLE:    return 0x201040;
    case RING_MAGENTA:   return 0x400040;
    case RING_SYNTHWAVE: return 0x201040;
    case RING_OFF:       return 0x000000;
    case RING_CYAN:
    default:             return 0x004A55;
  }
}

uint32_t getRingMainColor(int mode)
{
  switch (wrapRingColor(mode))
  {
    case RING_BLUE:      return 0x0095FF;
    case RING_RED:       return 0xFF0000;
    case RING_YELLOW:    return 0xFFFF00;
    case RING_GREEN:     return 0x00FF00;
    case RING_PURPLE:    return 0x8000FF;
    case RING_MAGENTA:   return 0xFF00FF;
    case RING_SYNTHWAVE: return 0xFF00FF;
    case RING_OFF:       return 0x000000;
    case RING_CYAN:
    default:             return 0x00FFFF;
  }
}

uint32_t getRingHighlightColor(int mode)
{
  switch (wrapRingColor(mode))
  {
    case RING_BLUE:      return 0xAAFFFF;
    case RING_RED:       return 0xFF8888;
    case RING_YELLOW:    return 0xFFF6AA;
    case RING_GREEN:     return 0xAAFFAA;
    case RING_PURPLE:    return 0xC080FF;
    case RING_MAGENTA:   return 0xFFAAFF;
    case RING_SYNTHWAVE: return 0x00FFFF;
    case RING_OFF:       return 0xFFFFFF;
    case RING_CYAN:
    default:             return 0xAAFFFF;
  }
}

uint32_t getRingInnerColor(int mode)
{
  switch (wrapRingColor(mode))
  {
    case RING_BLUE:      return 0x0033FF;
    case RING_RED:       return 0x990000;
    case RING_YELLOW:    return 0xBBAA00;
    case RING_GREEN:     return 0x008800;
    case RING_PURPLE:    return 0x400088;
    case RING_MAGENTA:   return 0x880088;
    case RING_SYNTHWAVE: return 0x8000FF;
    case RING_OFF:       return 0x000000;
    case RING_CYAN:
    default:             return 0x0095FF;
  }
}

// =====================================================
// Ring Drawing
// =====================================================

void drawRings()
{
  clearLayer(ringLayer);

  int mode = getCurrentRingColor();

  // No color / ring-off mode keeps the screen clean with no outer light ring.
  if (mode == RING_OFF)
  {
    return;
  }

  uint32_t glow = getRingGlowColor(mode);
  uint32_t main = getRingMainColor(mode);
  uint32_t highlight = getRingHighlightColor(mode);
  uint32_t inner = getRingInnerColor(mode);

  createRing(ringLayer, 456, 8, glow, LV_OPA_50);
  createRing(ringLayer, 446, 5, main, LV_OPA_COVER);
  createRing(ringLayer, 438, 2, highlight, LV_OPA_COVER);
  createRing(ringLayer, 430, 3, inner, LV_OPA_90);

  if (mode == RING_SYNTHWAVE)
  {
    createRing(ringLayer, 422, 2, 0x00FFFF, LV_OPA_70);
    createRing(ringLayer, 414, 2, 0x8000FF, LV_OPA_70);
  }
}

// =====================================================
// Shift Lights
// =====================================================

uint32_t getShiftLightColor(int index)
{
  if (index <= 3 || index >= 18) return 0x00FF00;
  if ((index >= 4 && index <= 6) || (index >= 15 && index <= 17)) return 0xFFFF00;
  if ((index >= 7 && index <= 8) || (index >= 13 && index <= 14)) return 0xFF0000;
  return 0x0095FF;
}

bool isShiftLightLit(int index, int rpm)
{
  if ((index == 0 || index == 21) && rpm >= shiftLightRpm[0]) return true;
  if ((index == 1 || index == 20) && rpm >= shiftLightRpm[1]) return true;
  if ((index == 2 || index == 19) && rpm >= shiftLightRpm[2]) return true;
  if ((index == 3 || index == 18) && rpm >= shiftLightRpm[3]) return true;

  if ((index == 4 || index == 17) && rpm >= shiftLightRpm[4]) return true;
  if ((index == 5 || index == 16) && rpm >= shiftLightRpm[5]) return true;
  if ((index == 6 || index == 15) && rpm >= shiftLightRpm[6]) return true;

  if ((index == 7 || index == 14) && rpm >= shiftLightRpm[7]) return true;
  if ((index == 8 || index == 13) && rpm >= shiftLightRpm[8]) return true;

  // Step 9 / final blue shift lights.
  // In shift setup they stay solid so the outer blue circles are visible.
  // On the RPM screen they flash at 4 flashes per second.
  if (rpm >= shiftLightRpm[9] && index >= 9 && index <= 12)
  {
    if (currentScreen == SCREEN_SHIFT_SETTINGS)
    {
      return true;
    }

    bool flashOn = ((millis() / 125) % 2) == 0;
    return flashOn;
  }

  return false;
}


void drawShiftLights(int rpm, bool showLights)
{
  const int DOT_COUNT = 22;
  const float START_DEG = 180.0;
  const float END_DEG = 360.0;
  const float RADIUS = 188.0;

  for (int i = 0; i < DOT_COUNT; i++)
  {
    float stepDeg = (END_DEG - START_DEG) / (DOT_COUNT - 1);
    float deg = START_DEG + i * stepDeg;
    float rad = deg * 0.01745329252f;

    int x = (int)(cos(rad) * RADIUS);
    int y = (int)(sin(rad) * RADIUS);

    uint32_t color = getShiftLightColor(i);
    bool lit = showLights && isShiftLightLit(i, rpm);

    if (lit)
    {
      createDot(contentLayer, x, y, 24, color, LV_OPA_50);
      createDot(contentLayer, x, y, 16, color, LV_OPA_COVER);
    }
    else
    {
      createDot(contentLayer, x, y, 14, 0x252525, LV_OPA_COVER);
    }
  }
}

// =====================================================
// Gauge Screens
// =====================================================

void drawAllInOneDivider()
{
  int mode = getCurrentRingColor();
  uint32_t main = getRingMainColor(mode);

  // Separator line between the top RPM/speed area and bottom data area.
  createSolidRect(contentLayer, 300, 3, 0, 22, main, LV_OPA_80, 2);

  // Bottom vertical split between throttle and intake air.
  createSolidRect(contentLayer, 3, 120, 0, 82, main, LV_OPA_60, 2);
}

void drawAllInOneThrottleBars(int percent, bool hasData, uint32_t activeColor)
{
  // Larger throttle bar visuals, matching the individual throttle gauge proportions.
  const int segmentCount = 10;
  const int segmentW = 28;
  const int segmentH = 14;
  const int gap = 7;
  const int totalW = segmentCount * segmentW + (segmentCount - 1) * gap;

  // Center the full bar group underneath the centered MPH number.
  const int startX = -(totalW / 2) + (segmentW / 2);

  // Align with the first RPM shift-light step horizontal axis.
  const int barY = 0;

  int filledSegments = 0;

  if (hasData)
  {
    filledSegments = (percent + 9) / 10;

    if (percent > 0 && filledSegments < 1)
    {
      filledSegments = 1;
    }

    if (filledSegments > segmentCount)
    {
      filledSegments = segmentCount;
    }
  }

  for (int i = 0; i < segmentCount; i++)
  {
    uint32_t color = 0x252525;

    if (hasData && i < filledSegments)
    {
      color = activeColor;
    }

    createSolidRect(
      contentLayer,
      segmentW,
      segmentH,
      startX + i * (segmentW + gap),
      barY,
      color,
      LV_OPA_COVER,
      5
    );
  }
}

void showAllInOneGauge()
{
  clearLayer(contentLayer);

  int mode = getCurrentRingColor();
  uint32_t highlight = getRingHighlightColor(mode);
  uint32_t main = getRingMainColor(mode);

  // Use the existing RPM shift-light arc as the visual template.
  drawShiftLights(liveRpm, hasLiveRpm);

  // No "BlueLine One" text here; keep the screen cleaner.

  // RPM number removed from All-In-One.
  // Shift lights remain active and still use saved shift-step preferences.

  int shownSpeed = 0;

  if (hasLiveSpeed)
  {
    if (speedUnitMode == SPEED_UNIT_KPH)
    {
      shownSpeed = liveSpeedKph;
    }
    else
    {
      shownSpeed = (int)(liveSpeedKph * 0.621371 + 0.5);
    }
  }

  char speedText[16];
  const char *speedUnit = speedUnitMode == SPEED_UNIT_KPH ? "KPH" : "MPH";

  if (hasLiveSpeed)
  {
    snprintf(speedText, sizeof(speedText), "%d", shownSpeed);
  }
  else
  {
    snprintf(speedText, sizeof(speedText), "---");
  }

  // Speed number stays centered; MPH/KPH is smaller and offset to the right.
  createLabel(contentLayer, speedText, &Xolonium_pn4D72pt7b, hasLiveSpeed ? 0xFFFFFF : 0x666666, 0, -66);
  createLabel(contentLayer, speedUnit, &Xolonium_pn4D24pt7b, highlight, 96, -58);

  drawAllInOneDivider();

  // Bottom left: throttle with % and horizontal bars.
  uint32_t throttleColor = getThrottleColor(liveThrottlePercent, hasLiveThrottle);

  createLabel(contentLayer, "THROTTLE", &Xolonium_pn4D18pt7b, main, -105, 44);

  char throttleText[18];

  if (hasLiveThrottle)
  {
    snprintf(throttleText, sizeof(throttleText), "%d%%", liveThrottlePercent);
  }
  else
  {
    snprintf(throttleText, sizeof(throttleText), "---%%");
  }

  createLabel(contentLayer, throttleText, &Xolonium_pn4D48pt7b, throttleColor, -105, 96);
  drawAllInOneThrottleBars(liveThrottlePercent, hasLiveThrottle, throttleColor);

  // Bottom right: intake air, no symbol.
  int intakeAirF = 0;
  int shownTemp = 0;
  const char *degreeSymbol = "\xC2\xB0";
  const char *unitText = intakeAirUnitMode == TEMP_UNIT_C ? "C" : "F";

  if (hasLiveIntakeAirTemp)
  {
    intakeAirF = (int)((liveIntakeAirTempC * 9.0 / 5.0) + 32.0 + 0.5);

    if (intakeAirUnitMode == TEMP_UNIT_C)
    {
      shownTemp = liveIntakeAirTempC;
    }
    else
    {
      shownTemp = intakeAirF;
    }
  }

  uint32_t intakeColor = getIntakeAirColor(intakeAirF, hasLiveIntakeAirTemp);

  createLabel(contentLayer, "INTAKE AIR", &Xolonium_pn4D18pt7b, main, 108, 44);

  char intakeText[20];

  if (hasLiveIntakeAirTemp)
  {
    snprintf(intakeText, sizeof(intakeText), "%d%s%s", shownTemp, degreeSymbol, unitText);
  }
  else
  {
    snprintf(intakeText, sizeof(intakeText), "---%s%s", degreeSymbol, unitText);
  }

  createLabel(contentLayer, intakeText, &Xolonium_pn4D48pt7b, intakeColor, 108, 96);

  drawBluetoothIcon();
}

void showBatteryGauge()
{
  clearLayer(contentLayer);

  int mode = getCurrentRingColor();
  uint32_t highlight = getRingHighlightColor(mode);

  createLabel(contentLayer, "BATTERY", &Xolonium_pn4D32pt7b, highlight, 0, -120);

  char voltageText[16];

  if (hasLiveVoltage)
  {
    snprintf(voltageText, sizeof(voltageText), "%.1fv", liveBatteryVoltage);
  }
  else
  {
    snprintf(voltageText, sizeof(voltageText), "--.-v");
  }

  uint32_t voltageColor = 0xFFFFFF;

  if (!hasLiveVoltage)
  {
    voltageColor = 0x666666;
  }
  else if (liveBatteryVoltage < 12.0)
  {
    voltageColor = 0xFF0000;     // low / problem
  }
  else if (liveBatteryVoltage < 12.6)
  {
    voltageColor = 0xFFFF00;     // weak battery voltage
  }
  else if (liveBatteryVoltage < 13.2)
  {
    voltageColor = 0x00FF00;     // healthy battery voltage
  }
  else
  {
    voltageColor = 0x0095FF;     // alternator charging blue
  }

  createLabel(contentLayer, voltageText, &Xolonium_pn4D72pt7b, voltageColor, 0, -30);

  // =====================================================
  // White dashboard battery icon with 1/4 segmented fill
  // =====================================================

  const uint32_t iconWhite = 0xFFFFFF;
  const uint32_t fillColor = voltageColor;

  const int batteryY = 78;

  const int bodyW = 180;
  const int bodyH = 90;

  const int postW = 34;
  const int postH = 18;

  const int leftPostX = -52;
  const int rightPostX = 52;
  const int postY = batteryY - (bodyH / 2) - (postH / 2) + 3;

  const int innerPad = 11;
  const int innerW = bodyW - innerPad * 2;
  const int innerH = bodyH - innerPad * 2;

  // Interior fill in 1/4 segments. Tuned to this Mazda's normal charging range.
  if (hasLiveVoltage)
  {
    int filledSegments = 0;

    if (liveBatteryVoltage >= 12.0) filledSegments = 1;
    if (liveBatteryVoltage >= 12.5) filledSegments = 2;
    if (liveBatteryVoltage >= 13.0) filledSegments = 3;
    if (liveBatteryVoltage >= 13.4) filledSegments = 4;

    const int segmentGap = 5;
    const int segmentW = (innerW - segmentGap * 3) / 4;

    for (int i = 0; i < filledSegments; i++)
    {
      int segmentX = -innerW / 2 + segmentW / 2 + i * (segmentW + segmentGap);

      lv_obj_t *segment = lv_obj_create(contentLayer);
      lv_obj_set_size(segment, segmentW, innerH);
      lv_obj_align(segment, LV_ALIGN_CENTER, segmentX, batteryY);
      lv_obj_set_style_bg_color(segment, lv_color_hex(fillColor), LV_PART_MAIN);
      lv_obj_set_style_bg_opa(segment, LV_OPA_80, LV_PART_MAIN);
      lv_obj_set_style_border_width(segment, 0, LV_PART_MAIN);
      lv_obj_set_style_radius(segment, 3, LV_PART_MAIN);
      lv_obj_set_style_pad_all(segment, 0, LV_PART_MAIN);
      lv_obj_clear_flag(segment, LV_OBJ_FLAG_SCROLLABLE);
    }
  }

  // Main white battery outline
  lv_obj_t *body = lv_obj_create(contentLayer);
  lv_obj_set_size(body, bodyW, bodyH);
  lv_obj_align(body, LV_ALIGN_CENTER, 0, batteryY);
  lv_obj_set_style_bg_opa(body, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_color(body, lv_color_hex(iconWhite), LV_PART_MAIN);
  lv_obj_set_style_border_width(body, 5, LV_PART_MAIN);
  lv_obj_set_style_radius(body, 8, LV_PART_MAIN);
  lv_obj_set_style_pad_all(body, 0, LV_PART_MAIN);
  lv_obj_clear_flag(body, LV_OBJ_FLAG_SCROLLABLE);

  // Left top post
  createSolidRect(contentLayer, postW, postH, leftPostX, postY, iconWhite, LV_OPA_COVER, 2);

  // Right top post
  createSolidRect(contentLayer, postW, postH, rightPostX, postY, iconWhite, LV_OPA_COVER, 2);

  // Minus sign, left
  createSolidRect(contentLayer, 38, 10, -48, batteryY, iconWhite, LV_OPA_COVER, 2);

  // Plus sign, right
  createSolidRect(contentLayer, 38, 10, 48, batteryY, iconWhite, LV_OPA_COVER, 2);
  createSolidRect(contentLayer, 10, 38, 48, batteryY, iconWhite, LV_OPA_COVER, 2);

  drawBluetoothIcon();
}

void showRpmGauge()
{
  clearLayer(contentLayer);

  drawShiftLights(liveRpm, hasLiveRpm);

  createLabel(contentLayer, "RPM", &Xolonium_pn4D32pt7b, 0xFFFFFF, 0, -120);

  char rpmText[16];

  if (hasLiveRpm)
  {
    snprintf(rpmText, sizeof(rpmText), "%d", liveRpm);
  }
  else
  {
    snprintf(rpmText, sizeof(rpmText), "----");
  }

  uint32_t rpmColor = hasLiveRpm ? 0xFFFFFF : 0x666666;

  createLabel(contentLayer, rpmText, &Xolonium_pn4D72pt7b, rpmColor, 0, -25);

  drawGearIcon();
  drawBluetoothIcon();
}

void showSpeedGauge()
{
  clearLayer(contentLayer);

  int mode = getCurrentRingColor();
  uint32_t main = getRingMainColor(mode);
  uint32_t highlight = getRingHighlightColor(mode);

  createLabel(contentLayer, "SPEED", &Xolonium_pn4D32pt7b, highlight, 0, -120);

  int shownSpeed = 0;

  if (hasLiveSpeed)
  {
    if (speedUnitMode == SPEED_UNIT_KPH)
    {
      shownSpeed = liveSpeedKph;
    }
    else
    {
      shownSpeed = (int)(liveSpeedKph * 0.621371 + 0.5);
    }
  }

  char speedText[16];

  if (hasLiveSpeed)
  {
    snprintf(speedText, sizeof(speedText), "%d", shownSpeed);
  }
  else
  {
    snprintf(speedText, sizeof(speedText), "---");
  }

  createLabel(contentLayer, speedText, &Xolonium_pn4D72pt7b, hasLiveSpeed ? main : 0x555555, 0, -30);

  const char *unit = speedUnitMode == SPEED_UNIT_MPH ? "MPH" : "KPH";

  createLabel(contentLayer, unit, &Xolonium_pn4D32pt7b, 0x00FF80, 0, 55);

  drawBluetoothIcon();
}

void showCoolantGauge()
{
  clearLayer(contentLayer);

  int mode = getCurrentRingColor();
  uint32_t highlight = getRingHighlightColor(mode);

  createLabel(contentLayer, "COOLANT", &Xolonium_pn4D32pt7b, highlight, 0, -120);

  int coolantF = 0;
  int shownTemp = 0;
  uint32_t tempColor = 0x666666;
  const char *degreeSymbol = "\xC2\xB0";
  const char *unitText = coolantUnitMode == TEMP_UNIT_C ? "C" : "F";

  if (hasLiveCoolantTemp)
  {
    coolantF = (int)((liveCoolantTempC * 9.0 / 5.0) + 32.0 + 0.5);

    if (coolantUnitMode == TEMP_UNIT_C)
    {
      shownTemp = liveCoolantTempC;
    }
    else
    {
      shownTemp = coolantF;
    }

    // Color thresholds are based on Fahrenheit engine temp ranges.
    if (coolantF < 140)
    {
      tempColor = 0x0095FF;
    }
    else if (coolantF < 220)
    {
      tempColor = 0x00FF00;
    }
    else if (coolantF < 235)
    {
      tempColor = 0xFFFF00;
    }
    else
    {
      tempColor = 0xFF0000;
    }
  }

  char tempText[20];

  if (hasLiveCoolantTemp)
  {
    snprintf(tempText, sizeof(tempText), "%d%s%s", shownTemp, degreeSymbol, unitText);
  }
  else
  {
    snprintf(tempText, sizeof(tempText), "---%s%s", degreeSymbol, unitText);
  }

  createLabel(contentLayer, tempText, &Xolonium_pn4D72pt7b, tempColor, 0, -40);

  drawCoolantIcon(tempColor);
  drawBluetoothIcon();
}

uint32_t getThrottleColor(int percent, bool hasData)
{
  if (!hasData)
  {
    return 0x666666;
  }

  if (percent <= 25)
  {
    return 0x00FFFF;
  }

  if (percent <= 60)
  {
    return 0x00FF00;
  }

  if (percent <= 85)
  {
    return 0xFFFF00;
  }

  return 0xFF0000;
}

void drawThrottleSegments(int percent, bool hasData, uint32_t activeColor)
{
  const int segmentCount = 10;
  const int segmentW = 28;
  const int segmentH = 14;
  const int gap = 7;
  const int totalW = segmentCount * segmentW + (segmentCount - 1) * gap;
  const int startX = -(totalW / 2) + (segmentW / 2);
  const int barY = 75;

  int filledSegments = 0;

  if (hasData)
  {
    filledSegments = (percent + 9) / 10;

    if (percent > 0 && filledSegments < 1)
    {
      filledSegments = 1;
    }

    if (filledSegments > segmentCount)
    {
      filledSegments = segmentCount;
    }
  }

  for (int i = 0; i < segmentCount; i++)
  {
    uint32_t color = 0x252525;
    lv_opa_t opacity = LV_OPA_COVER;

    if (hasData && i < filledSegments)
    {
      color = activeColor;
      opacity = LV_OPA_COVER;
    }

    int x = startX + i * (segmentW + gap);
    createSolidRect(contentLayer, segmentW, segmentH, x, barY, color, opacity, 5);
  }
}

void showThrottleGauge()
{
  clearLayer(contentLayer);

  int mode = getCurrentRingColor();
  uint32_t highlight = getRingHighlightColor(mode);

  createLabel(contentLayer, "THROTTLE", &Xolonium_pn4D32pt7b, highlight, 0, -120);

  char throttleText[16];

  if (hasLiveThrottle)
  {
    snprintf(throttleText, sizeof(throttleText), "%d%%", liveThrottlePercent);
  }
  else
  {
    snprintf(throttleText, sizeof(throttleText), "---%%");
  }

  uint32_t throttleColor = getThrottleColor(liveThrottlePercent, hasLiveThrottle);

  createLabel(contentLayer, throttleText, &Xolonium_pn4D72pt7b, throttleColor, 0, -30);
  drawThrottleSegments(liveThrottlePercent, hasLiveThrottle, throttleColor);
  drawBluetoothIcon();
}

uint32_t getIntakeAirColor(int intakeAirF, bool hasData)
{
  if (!hasData)
  {
    return 0x666666;
  }

  if (intakeAirF < 50)
  {
    return 0x0095FF;
  }

  if (intakeAirF <= 100)
  {
    return 0x00FF80;
  }

  if (intakeAirF <= 130)
  {
    return 0xFFFF00;
  }

  return 0xFF0000;
}

void showIntakeAirGauge()
{
  clearLayer(contentLayer);

  int mode = getCurrentRingColor();
  uint32_t highlight = getRingHighlightColor(mode);

  createLabel(contentLayer, "INTAKE AIR", &Xolonium_pn4D32pt7b, highlight, 0, -120);

  int intakeAirF = 0;
  int shownTemp = 0;
  const char *degreeSymbol = "\xC2\xB0";
  const char *unitText = intakeAirUnitMode == TEMP_UNIT_C ? "C" : "F";

  if (hasLiveIntakeAirTemp)
  {
    intakeAirF = (int)((liveIntakeAirTempC * 9.0 / 5.0) + 32.0 + 0.5);

    if (intakeAirUnitMode == TEMP_UNIT_C)
    {
      shownTemp = liveIntakeAirTempC;
    }
    else
    {
      shownTemp = intakeAirF;
    }
  }

  uint32_t tempColor = getIntakeAirColor(intakeAirF, hasLiveIntakeAirTemp);

  char tempText[20];

  if (hasLiveIntakeAirTemp)
  {
    snprintf(tempText, sizeof(tempText), "%d%s%s", shownTemp, degreeSymbol, unitText);
  }
  else
  {
    snprintf(tempText, sizeof(tempText), "---%s%s", degreeSymbol, unitText);
  }

  createLabel(contentLayer, tempText, &Xolonium_pn4D72pt7b, tempColor, 0, -40);

  drawIntakeAirIcon(tempColor);
  drawBluetoothIcon();
}

uint32_t getAirflowColor(float mafGps, bool hasData)
{
  if (!hasData)
  {
    return 0x666666;
  }

  if (mafGps < 5.0)
  {
    return 0x00FFFF;
  }

  if (mafGps < 40.0)
  {
    return 0x00FF80;
  }

  if (mafGps < 100.0)
  {
    return 0xFFFF00;
  }

  return 0xFF0000;
}

void drawAirflowIcon(uint32_t iconColor)
{
  const int iconY = 78;

  // Airflow/turbine icon made from simple LVGL shapes.
  // Left air stream lines
  createSolidRect(contentLayer, 78, 7, -62, iconY - 24, iconColor, LV_OPA_70, 3);
  createSolidRect(contentLayer, 95, 7, -70, iconY,      iconColor, LV_OPA_90, 3);
  createSolidRect(contentLayer, 78, 7, -62, iconY + 24, iconColor, LV_OPA_70, 3);

  // Main turbine rings
  lv_obj_t *outer = lv_obj_create(contentLayer);
  lv_obj_set_size(outer, 88, 88);
  lv_obj_align(outer, LV_ALIGN_CENTER, 42, iconY);
  lv_obj_set_style_bg_opa(outer, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_color(outer, lv_color_hex(iconColor), LV_PART_MAIN);
  lv_obj_set_style_border_width(outer, 4, LV_PART_MAIN);
  lv_obj_set_style_border_opa(outer, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_radius(outer, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_pad_all(outer, 0, LV_PART_MAIN);
  lv_obj_clear_flag(outer, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *inner = lv_obj_create(contentLayer);
  lv_obj_set_size(inner, 56, 56);
  lv_obj_align(inner, LV_ALIGN_CENTER, 42, iconY);
  lv_obj_set_style_bg_opa(inner, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_color(inner, lv_color_hex(iconColor), LV_PART_MAIN);
  lv_obj_set_style_border_width(inner, 3, LV_PART_MAIN);
  lv_obj_set_style_border_opa(inner, LV_OPA_70, LV_PART_MAIN);
  lv_obj_set_style_radius(inner, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_pad_all(inner, 0, LV_PART_MAIN);
  lv_obj_clear_flag(inner, LV_OBJ_FLAG_SCROLLABLE);

  createDot(contentLayer, 42, iconY, 18, iconColor, LV_OPA_COVER);

  // Turbine blades
  createSolidRect(contentLayer, 8, 26, 42, iconY - 26, iconColor, LV_OPA_COVER, 3);
  createSolidRect(contentLayer, 8, 26, 42, iconY + 26, iconColor, LV_OPA_COVER, 3);
  createSolidRect(contentLayer, 26, 8, 16, iconY, iconColor, LV_OPA_COVER, 3);
  createSolidRect(contentLayer, 26, 8, 68, iconY, iconColor, LV_OPA_COVER, 3);
}

void showAirflowGauge()
{
  clearLayer(contentLayer);

  int mode = getCurrentRingColor();
  uint32_t highlight = getRingHighlightColor(mode);

  createLabel(contentLayer, "AIRFLOW", &Xolonium_pn4D32pt7b, highlight, 0, -120);

  uint32_t airflowColor = getAirflowColor(liveMafGps, hasLiveMaf);

  char mafText[24];

  if (hasLiveMaf)
  {
    snprintf(mafText, sizeof(mafText), "%.1f g/s", liveMafGps);
  }
  else
  {
    snprintf(mafText, sizeof(mafText), "--.- g/s");
  }

  // MAF value and unit together, same 72pt font, centered like the other gauges.
  createLabel(contentLayer, mafText, &Xolonium_pn4D72pt7b, airflowColor, 0, -30);

  drawAirflowIcon(airflowColor);
  drawBluetoothIcon();
}

uint32_t getMpgColor(float mpg, bool hasData)
{
  if (!hasData)
  {
    return 0x666666;
  }

  if (mpg < 15.0)
  {
    return 0xFF0000;
  }

  if (mpg < 25.0)
  {
    return 0xFFFF00;
  }

  if (mpg < 35.0)
  {
    return 0x00FF00;
  }

  return 0x0095FF;
}

bool calculateInstantMpg(float &mpgOut)
{
  if (!hasLiveSpeed || !hasLiveMaf)
  {
    return false;
  }

  float mph = liveSpeedKph * 0.621371;

  if (mph < 3.0 || liveMafGps <= 0.05)
  {
    return false;
  }

  float mpg = (mph * 11.43) / liveMafGps;

  if (mpg < 0.0 || mpg > 250.0)
  {
    return false;
  }

  if (mpg > 99.9)
  {
    mpg = 99.9;
  }

  mpgOut = mpg;
  return true;
}

void drawMpgIcon(uint32_t iconColor)
{
  // Simple road/efficiency icon under the MPG value.
  createSolidRect(contentLayer, 88, 6, 0, 82, iconColor, LV_OPA_COVER, 3);
  createSolidRect(contentLayer, 52, 5, 0, 101, iconColor, LV_OPA_70, 3);
  createSolidRect(contentLayer, 18, 5, -35, 101, iconColor, LV_OPA_30, 3);
  createSolidRect(contentLayer, 18, 5, 35, 101, iconColor, LV_OPA_30, 3);
}

void showMpgGauge()
{
  clearLayer(contentLayer);

  int mode = getCurrentRingColor();
  uint32_t highlight = getRingHighlightColor(mode);

  createLabel(contentLayer, "MPG", &Xolonium_pn4D32pt7b, highlight, 0, -120);

  // Top value: instant MPG in 72pt, centered with no label.
  float instantMpg = 0.0;
  bool hasInstantMpg = calculateInstantMpg(instantMpg);
  uint32_t instantColor = getMpgColor(instantMpg, hasInstantMpg);

  char instantText[16];

  if (hasInstantMpg)
  {
    snprintf(instantText, sizeof(instantText), "%.1f", instantMpg);
  }
  else
  {
    snprintf(instantText, sizeof(instantText), "--.-");
  }

  createLabel(contentLayer, instantText, &Xolonium_pn4D72pt7b, instantColor, 0, -60);

  // Lower value: average MPG with "avg" inline, centered as one label.
  float averageMpg = 0.0;
  bool hasAverageMpg = calculateAverageMpg(averageMpg);
  uint32_t averageColor = getMpgColor(averageMpg, hasAverageMpg);

  char averageText[24];

  if (hasAverageMpg)
  {
    snprintf(averageText, sizeof(averageText), "%.1f avg", averageMpg);
  }
  else
  {
    snprintf(averageText, sizeof(averageText), "--.- avg");
  }

  createLabel(contentLayer, averageText, &Xolonium_pn4D60pt7b, averageColor, 0, 35);

  drawMpgIcon(averageColor);
  drawBluetoothIcon();
}


float getLiveSpeedMph()
{
  if (!hasLiveSpeed)
  {
    return 0.0;
  }

  return liveSpeedKph * 0.621371;
}

float getCurrentMpgEstimate(bool &hasMpg)
{
  float mpg = 0.0;
  hasMpg = calculateAverageMpg(mpg);
  return mpg;
}

bool calculateAverageMpg(float &mpgOut)
{
  if (mpgTripGallons <= 0.0001)
  {
    return false;
  }

  float mpg = mpgTripMiles / mpgTripGallons;

  if (mpg < 0.0 || mpg > 250.0)
  {
    return false;
  }

  if (mpg > 99.9)
  {
    mpg = 99.9;
  }

  mpgOut = mpg;
  return true;
}

void updateAverageMpg()
{
  unsigned long now = millis();

  if (lastMpgAverageUpdateTime == 0)
  {
    lastMpgAverageUpdateTime = now;
    return;
  }

  if (now - lastMpgAverageUpdateTime < MPG_AVERAGE_UPDATE_INTERVAL)
  {
    return;
  }

  unsigned long deltaMs = now - lastMpgAverageUpdateTime;
  lastMpgAverageUpdateTime = now;

  // Ignore huge gaps from screen changes, BLE reconnects, or pauses.
  if (deltaMs > 5000)
  {
    return;
  }

  if (!hasLiveSpeed || !hasLiveMaf)
  {
    return;
  }

  float mph = liveSpeedKph * 0.621371;

  // Do not count idle or very low speed against the trip MPG.
  // At idle, the screen keeps showing the last saved/running average.
  if (mph < 3.0 || liveMafGps <= 0.05)
  {
    return;
  }

  float deltaHours = (float)deltaMs / 3600000.0;
  float gallonsPerHour = liveMafGps / 11.43;

  if (gallonsPerHour <= 0.0 || gallonsPerHour > 50.0)
  {
    return;
  }

  mpgTripMiles += mph * deltaHours;
  mpgTripGallons += gallonsPerHour * deltaHours;

  if (now - lastMpgAverageSaveTime >= MPG_AVERAGE_SAVE_INTERVAL)
  {
    lastMpgAverageSaveTime = now;
    saveMpgAverageSettings();
  }
}

void resetAverageMpg()
{
  mpgTripMiles = 0.0;
  mpgTripGallons = 0.0;
  lastMpgAverageUpdateTime = millis();
  lastMpgAverageSaveTime = millis();
  saveMpgAverageSettings();
  redrawUI();
}

unsigned long getChronographElapsedMs()
{
  if (chronoRunning)
  {
    return chronoElapsedMillis + (millis() - chronoStartMillis);
  }

  return chronoElapsedMillis;
}

const char *getChronographModeText()
{
  if (chronoMode == CHRONO_MODE_30_70) return "30-70 MPH";
  if (chronoMode == CHRONO_MODE_60_100) return "60-100 MPH";
  return "0-60 MPH";
}

const char *getChronographStatusText()
{
  if (chronoRunning) return "RUN";
  if (chronoElapsedMillis > 0) return "STOP";
  return "READY";
}

void toggleChronograph()
{
  if (currentScreen != SCREEN_CHRONO)
  {
    return;
  }

  if (chronoRunning)
  {
    chronoElapsedMillis = getChronographElapsedMs();
    chronoRunning = false;
  }
  else
  {
    chronoStartMillis = millis();
    chronoRunning = true;
  }

  lastChronoVisualTick = 0xFFFFFFFF;
  lastChronoRedrawTime = 0;
  redrawUI();
}

void resetChronograph()
{
  if (currentScreen != SCREEN_CHRONO)
  {
    return;
  }

  chronoRunning = false;
  chronoElapsedMillis = 0;
  chronoStartMillis = millis();
  lastChronoVisualTick = 0xFFFFFFFF;
  lastChronoRedrawTime = 0;
  redrawUI();
}

void nextChronographMode()
{
  chronoMode++;
  if (chronoMode >= CHRONO_MODE_COUNT) chronoMode = 0;
  resetChronograph();
}

void previousChronographMode()
{
  chronoMode--;
  if (chronoMode < 0) chronoMode = CHRONO_MODE_COUNT - 1;
  resetChronograph();
}

void setupChronographButtons()
{
  if (CHRONO_POWER_BUTTON_PIN >= 0)
  {
    pinMode(CHRONO_POWER_BUTTON_PIN, INPUT_PULLUP);
    chronoPowerButtonLast = digitalRead(CHRONO_POWER_BUTTON_PIN);
  }

  if (CHRONO_BOOT_BUTTON_PIN >= 0)
  {
    pinMode(CHRONO_BOOT_BUTTON_PIN, INPUT_PULLUP);
    chronoBootButtonLast = digitalRead(CHRONO_BOOT_BUTTON_PIN);
  }

#if BLUELINE_HAS_XPOWERS
  // PWR button is exposed through the AXP2101 PMU as a short-press interrupt.
  chronoPowerReady = chronoPower.begin(Wire, AXP2101_SLAVE_ADDRESS, IIC_SDA, IIC_SCL);

  if (chronoPowerReady)
  {
    chronoPower.disableIRQ(XPOWERS_AXP2101_ALL_IRQ);
    chronoPower.clearIrqStatus();
    chronoPower.enableIRQ(XPOWERS_AXP2101_PKEY_SHORT_IRQ);
    Serial.println("Chronograph PWR button enabled through AXP2101 PMU");
  }
  else
  {
    Serial.println("Chronograph PWR button PMU init failed; touch start/stop still works");
  }
#else
  Serial.println("XPowersLib not found; PWR button start/stop unavailable");
#endif
}

void handleChronographButtons()
{
  if (currentScreen != SCREEN_CHRONO)
  {
    return;
  }

  unsigned long now = millis();

  if (now - lastChronoButtonTime < CHRONO_BUTTON_DEBOUNCE)
  {
    return;
  }

#if BLUELINE_HAS_XPOWERS
  // PWR button: Start / Stop through AXP2101 PMU short-press IRQ.
  if (chronoPowerReady)
  {
    chronoPower.getIrqStatus();

    if (chronoPower.isPekeyShortPressIrq())
    {
      lastChronoButtonTime = now;
      toggleChronograph();
      chronoPower.clearIrqStatus();
      return;
    }

    chronoPower.clearIrqStatus();
  }
#endif

  // Fallback direct GPIO PWR button path, only used if a readable GPIO is mapped.
  if (CHRONO_POWER_BUTTON_PIN >= 0)
  {
    bool powerState = digitalRead(CHRONO_POWER_BUTTON_PIN);
    if (chronoPowerButtonLast == true && powerState == false)
    {
      lastChronoButtonTime = now;
      chronoPowerButtonLast = powerState;
      toggleChronograph();
      return;
    }

    chronoPowerButtonLast = powerState;
  }

  // BOOT button: Reset chronograph.
  if (CHRONO_BOOT_BUTTON_PIN >= 0)
  {
    bool bootState = digitalRead(CHRONO_BOOT_BUTTON_PIN);
    if (chronoBootButtonLast == true && bootState == false)
    {
      lastChronoButtonTime = now;
      chronoBootButtonLast = bootState;
      resetChronograph();
      return;
    }

    chronoBootButtonLast = bootState;
  }
}

void updateChronograph()
{
  if (currentScreen != SCREEN_CHRONO || !chronoRunning)
  {
    return;
  }

  unsigned long now = millis();
  if (now - lastChronoRedrawTime < CHRONO_REDRAW_INTERVAL)
  {
    return;
  }

  lastChronoRedrawTime = now;

  // Stopwatch feel: the hand only visually advances once per second.
  // This avoids constantly redrawing the same hand position and makes it
  // behave more like a traditional chronograph.
  unsigned long elapsed = getChronographElapsedMs();
  unsigned long visualTick = elapsed / CHRONO_SECOND_HAND_TICK_MS;

  if (visualTick != lastChronoVisualTick)
  {
    lastChronoVisualTick = visualTick;
    redrawUI();
  }
}

void drawChronoSubdialNumber(int cx, int cy, const char *text, float angleDeg, int radius, uint32_t color)
{
  if (text == nullptr || strlen(text) == 0)
  {
    return;
  }

  float rad = (angleDeg - 90.0) * DEG_TO_RAD;
  int x = cx + (int)(cos(rad) * radius);
  int y = cy + (int)(sin(rad) * radius);

  createLabel(contentLayer, text, &Xolonium_pn4D18pt7b, color, x, y);
}

void drawChronoSubdial(int cx, int cy, int radius, const char *topText, const char *rightLowerText, const char *leftLowerText, uint32_t color, float handAngleDeg)
{
  lv_obj_t *dial = lv_obj_create(contentLayer);
  lv_obj_set_size(dial, radius * 2, radius * 2);
  lv_obj_align(dial, LV_ALIGN_CENTER, cx, cy);
  lv_obj_set_style_bg_opa(dial, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_color(dial, lv_color_hex(0x1E5B78), LV_PART_MAIN);
  lv_obj_set_style_border_width(dial, 2, LV_PART_MAIN);
  lv_obj_set_style_border_opa(dial, LV_OPA_80, LV_PART_MAIN);
  lv_obj_set_style_radius(dial, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  lv_obj_set_style_pad_all(dial, 0, LV_PART_MAIN);
  lv_obj_clear_flag(dial, LV_OBJ_FLAG_SCROLLABLE);

  for (int i = 0; i < 12; i++)
  {
    float angle = i * 30.0;
    float rad = (angle - 90.0) * DEG_TO_RAD;
    int x1 = cx + (int)(cos(rad) * (radius - 8));
    int y1 = cy + (int)(sin(rad) * (radius - 8));
    int x2 = cx + (int)(cos(rad) * (radius - 3));
    int y2 = cy + (int)(sin(rad) * (radius - 3));
    createChronoLine(x1, y1, x2, y2, 0xFFFFFF, 2, LV_OPA_70);
  }

  // Subdial numbers: top, lower-right, lower-left.
  drawChronoSubdialNumber(cx, cy, topText, 0.0, radius - 22, 0xFFFFFF);
  drawChronoSubdialNumber(cx, cy, rightLowerText, 120.0, radius - 22, 0xFFFFFF);
  drawChronoSubdialNumber(cx, cy, leftLowerText, 240.0, radius - 22, 0xFFFFFF);

  // Functional red subdial hand. 0 degrees parks at 12 o'clock.
  float rad = (handAngleDeg - 90.0) * DEG_TO_RAD;
  int x2 = cx + (int)(cos(rad) * (radius - 14));
  int y2 = cy + (int)(sin(rad) * (radius - 14));

  createChronoLine(cx, cy, x2, y2, 0xFF2020, 4, LV_OPA_COVER);
  createDot(contentLayer, cx, cy, 14, 0xFFFFFF, LV_OPA_COVER);
  createDot(contentLayer, cx, cy, 6, 0xFF2020, LV_OPA_COVER);
}

void drawChronoHand(float angleDeg, int length, uint32_t color, int width)
{
  float rad = (angleDeg - 90.0) * DEG_TO_RAD;
  int x2 = (int)(cos(rad) * length);
  int y2 = (int)(sin(rad) * length);
  int x1 = (int)(cos(rad + PI) * 28);
  int y1 = (int)(sin(rad + PI) * 28);

  createChronoLine(x1, y1, x2, y2, color, width, LV_OPA_COVER);
  createDot(contentLayer, 0, 0, 26, 0xCCCCCC, LV_OPA_COVER);
  createDot(contentLayer, 0, 0, 14, 0x151515, LV_OPA_COVER);
  createDot(contentLayer, 0, 0, 8, color, LV_OPA_COVER);
}

void drawChronoDial(uint32_t mainColor, uint32_t highlightColor)
{
  chronoLinePointIndex = 0;

  createRing(contentLayer, 430, 2, 0x0B2D44, LV_OPA_COVER);
  createRing(contentLayer, 404, 2, mainColor, LV_OPA_70);
  createRing(contentLayer, 382, 1, 0xFFFFFF, LV_OPA_50);

  // Tachymetre markers use the same formula-positioned scale as the numbers.
  // This replaces the old evenly-spaced 5-second / 5-minute dial markers.
  const int tachyMarkerValues[] = {
    60, 65, 70, 75, 80, 85, 90, 95, 100, 110,
    140, 150, 160, 180, 200, 240, 300, 400
  };

  for (int i = 0; i < (int)(sizeof(tachyMarkerValues) / sizeof(tachyMarkerValues[0])); i++)
  {
    int value = tachyMarkerValues[i];

    // Keep every labeled tachymetre marker consistent.
    // This makes 65 / 75 / 85 match 60 / 70 / 80 instead of looking like minor ticks.
    createTachymetreMarker(
      value,
      176,
      194,
      0xFFFFFF,
      3,
      LV_OPA_COVER
    );
  }

  // Accurate TAG-style tachymetre scale.
  // Numbers are placed from the tachymetre formula instead of even spacing:
  // value = 3600 / elapsed seconds.
  const int tachyRadius = 158;

  createTachymetreLabel("60",  60,  &Xolonium_pn4D24pt7b, 0xFFFFFF, 152);
  createTachymetreLabel("65",  65,  &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("70",  70,  &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("75",  75,  &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("80",  80,  &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("85",  85,  &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("90",  90,  &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("100", 100, &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("110", 110, &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("140", 140, &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("150", 150, &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("160", 160, &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("180", 180, &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("200", 200, &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("240", 240, &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("300", 300, &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);
  createTachymetreLabel("400", 400, &Xolonium_pn4D18pt7b, 0xFFFFFF, tachyRadius);

  // Top branding, inspired by the mockup.
  createLabel(contentLayer, "BlueLine One", &Xolonium_pn4D24pt7b, 0xFFFFFF, 0, -105);
  createLabel(contentLayer, "Chronograph", &Xolonium_pn4D18pt7b, 0xC8C8C8, 0, -74);

  // Bluetooth symbol is drawn at the bottom like the other gauges.
}

void showChronographGauge()
{
  clearLayer(contentLayer);

  int mode = getCurrentRingColor();
  uint32_t main = getRingMainColor(mode);
  uint32_t highlight = getRingHighlightColor(mode);

  drawChronoDial(main, highlight);

  unsigned long elapsed = getChronographElapsedMs();

  // Stopwatch-style ticking seconds hand.
  // 1 tick per second = one visual step every 1000 ms.
  // A full 60-second revolution has 60 ticks, so each tick moves 6 degrees.
  unsigned long tickedElapsed = (elapsed / CHRONO_SECOND_HAND_TICK_MS) * CHRONO_SECOND_HAND_TICK_MS;
  float handAngle = (float)(tickedElapsed % 60000UL) * 360.0 / 60000.0;

  // Functional watch-style subdials.
  // Left = 12-hour counter, right = 30-minute counter, bottom = 60-second counter.
  float elapsedHours = (float)tickedElapsed / 3600000.0;
  float elapsedMinutes = (float)tickedElapsed / 60000.0;
  float elapsedSeconds = (float)(tickedElapsed % 60000UL) / 1000.0;

  float hourHandAngle = fmod(elapsedHours, 12.0) * 30.0;
  float minuteHandAngle = fmod(elapsedMinutes, 30.0) * 12.0;
  float secondSubdialAngle = elapsedSeconds * 6.0;

  drawChronoSubdial(-82, 0, 54, "12", "4",  "8",  main, hourHandAngle);
  drawChronoSubdial(82, 0, 54, "30", "10", "20", main, minuteHandAngle);

  // Bottom seconds subdial moved up slightly with 60 at 12 o'clock.
  drawChronoSubdial(0, 92, 43, "60", "", "", highlight, secondSubdialAngle);

  // Draw the main red chronograph hand after the dial and subdials so it sits
  // on the foreground instead of being hidden under the subdial objects.
  drawChronoHand(handAngle, 166, 0xFF2020, 4);

  // Match the other gauges: Bluetooth symbol at the 6 o'clock area.
  // Keep it last so the Bluetooth icon stays readable at the bottom.
  drawBluetoothIcon();
}


uint32_t getTpmsColor(float psi, bool hasData)
{
  if (!hasData)
  {
    return 0x666666;
  }

  if (psi < 26.0)
  {
    return 0xFF0000;
  }

  if (psi < 29.0)
  {
    return 0x0095FF;
  }

  if (psi <= 36.0)
  {
    return 0xFFFFFF;
  }

  if (psi <= 40.0)
  {
    return 0xFFFF00;
  }

  return 0xFF0000;
}

uint32_t getTpmsStatusLineColor(float psi, bool hasData)
{
  if (!hasData)
  {
    return 0x666666;
  }

  // Status line colors:
  // Red = unsafe low/high
  // Yellow = caution
  // Green = normal
  if (psi < 26.0)
  {
    return 0xFF0000;
  }

  if (psi < 29.0)
  {
    return 0xFFFF00;
  }

  if (psi <= 36.0)
  {
    return 0x00FF00;
  }

  if (psi <= 40.0)
  {
    return 0xFFFF00;
  }

  return 0xFF0000;
}

void drawTpmsTire(int x, int y, const char *cornerLabel, float psi, bool hasData, uint32_t color)
{
  char valueText[16];

  if (hasData)
  {
    if (tpmsUnitMode == TPMS_UNIT_KPA)
    {
      int kpa = (int)(psi * 6.89476 + 0.5);
      snprintf(valueText, sizeof(valueText), "%d", kpa);
    }
    else
    {
      snprintf(valueText, sizeof(valueText), "%.1f", psi);
    }
  }
  else
  {
    if (tpmsUnitMode == TPMS_UNIT_KPA)
    {
      snprintf(valueText, sizeof(valueText), "---");
    }
    else
    {
      snprintf(valueText, sizeof(valueText), "--.-");
    }
  }

  createLabel(contentLayer, valueText, &Xolonium_pn4D48pt7b, color, x, y - 16);

  // Horizontal accent line directly under the pressure number / --.- value.
  createSolidRect(
    contentLayer,
    52,
    4,
    x,
    y + 22,
    getTpmsStatusLineColor(psi, hasData),
    LV_OPA_90,
    2
  );

  // Tire block placement tuned per corner to better match the reference layout.
  // FL moves right.
  // FR moves left.
  // RL moves down and right.
  // RR moves down and left.
  int tireX = x;
  int tireY = y < 0 ? y + 44 : y - 44;

  if (strcmp(cornerLabel, "FL") == 0)
  {
    tireX = x + 34;
  }
  else if (strcmp(cornerLabel, "FR") == 0)
  {
    tireX = x - 34;
  }
  else if (strcmp(cornerLabel, "RL") == 0)
  {
    tireX = x + 34;
    tireY = y + 44;
  }
  else if (strcmp(cornerLabel, "RR") == 0)
  {
    tireX = x - 34;
    tireY = y + 44;
  }

  lv_obj_t *tire = lv_obj_create(contentLayer);
  lv_obj_set_size(tire, 18, 42);
  lv_obj_align(tire, LV_ALIGN_CENTER, tireX, tireY);
  lv_obj_set_style_bg_color(tire, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(tire, hasData ? LV_OPA_COVER : LV_OPA_40, LV_PART_MAIN);
  lv_obj_set_style_border_width(tire, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(tire, 5, LV_PART_MAIN);
  lv_obj_set_style_pad_all(tire, 0, LV_PART_MAIN);
  lv_obj_clear_flag(tire, LV_OBJ_FLAG_SCROLLABLE);
}

void showTpmsGauge()
{
  clearLayer(contentLayer);

  int mode = getCurrentRingColor();
  uint32_t highlight = getRingHighlightColor(mode);

  createLabel(contentLayer, "TPMS", &Xolonium_pn4D32pt7b, highlight, 0, -136);

  const char *unitText = tpmsUnitMode == TPMS_UNIT_KPA ? "kPa" : "PSI";
  createLabel(contentLayer, unitText, &Xolonium_pn4D32pt7b, 0xFFFFFF, 0, 0);

  drawTpmsTire(-105, -80, "FL", liveTpmsPsi[0], hasLiveTpms[0], getTpmsColor(liveTpmsPsi[0], hasLiveTpms[0]));
  drawTpmsTire(105, -80, "FR", liveTpmsPsi[1], hasLiveTpms[1], getTpmsColor(liveTpmsPsi[1], hasLiveTpms[1]));
  drawTpmsTire(-105, 70, "RL", liveTpmsPsi[2], hasLiveTpms[2], getTpmsColor(liveTpmsPsi[2], hasLiveTpms[2]));
  drawTpmsTire(105, 70, "RR", liveTpmsPsi[3], hasLiveTpms[3], getTpmsColor(liveTpmsPsi[3], hasLiveTpms[3]));

  drawBluetoothIcon();
}

uint32_t getOilTempColor(int oilTempF, bool hasData)
{
  if (!hasData)
  {
    return 0x666666;
  }

  if (oilTempF < 160)
  {
    return 0x0095FF;
  }

  if (oilTempF <= 230)
  {
    return 0x00FF80;
  }

  if (oilTempF <= 260)
  {
    return 0xFFFF00;
  }

  return 0xFF0000;
}

void styleOilTempIconPart(lv_obj_t *obj, uint32_t color)
{
  lv_obj_set_style_bg_color(obj, lv_color_hex(color), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
  lv_obj_set_style_outline_width(obj, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
  lv_obj_clear_flag(obj, LV_OBJ_FLAG_SCROLLABLE);
}

void drawOilTempIcon(uint32_t iconColor)
{
  // Oil temperature symbol styled closer to the reference:
  // no outer circle, simple thermometer, and smoother wave lines.
  const int iconX = 0;
  const int iconY = 86;

  // Soft glow behind the symbol
  createDot(contentLayer, iconX, iconY, 110, iconColor, LV_OPA_10);

  // Thermometer stem
  lv_obj_t *stem = lv_obj_create(contentLayer);
  lv_obj_set_size(stem, 18, 62);
  lv_obj_align(stem, LV_ALIGN_CENTER, iconX - 8, iconY - 18);
  lv_obj_set_style_radius(stem, 9, LV_PART_MAIN);
  styleOilTempIconPart(stem, iconColor);

  // Thermometer bulb
  lv_obj_t *bulb = lv_obj_create(contentLayer);
  lv_obj_set_size(bulb, 30, 30);
  lv_obj_align(bulb, LV_ALIGN_CENTER, iconX - 8, iconY + 22);
  lv_obj_set_style_radius(bulb, LV_RADIUS_CIRCLE, LV_PART_MAIN);
  styleOilTempIconPart(bulb, iconColor);

  // Temperature tick marks on the right side of the thermometer
  createSolidRect(contentLayer, 26, 7, iconX + 18, iconY - 35, iconColor, LV_OPA_COVER, 4);
  createSolidRect(contentLayer, 22, 7, iconX + 16, iconY - 18, iconColor, LV_OPA_COVER, 4);
  createSolidRect(contentLayer, 26, 7, iconX + 18, iconY - 1,  iconColor, LV_OPA_COVER, 4);

  // Smoother, less-squiggly wave lines
  static lv_point_precise_t smallWavePts[] = {
    {0, 4}, {10, 1}, {20, 4}, {30, 1}, {40, 4}
  };

  static lv_point_precise_t bottomWavePts[] = {
    {0, 5}, {10, 2}, {20, 5}, {30, 2}, {40, 5},
    {50, 2}, {60, 5}, {70, 2}, {80, 5}
  };

  // Left small wave
  lv_obj_t *leftWave = lv_line_create(contentLayer);
  lv_line_set_points(leftWave, smallWavePts, sizeof(smallWavePts) / sizeof(smallWavePts[0]));
  lv_obj_set_size(leftWave, 40, 8);
  lv_obj_align(leftWave, LV_ALIGN_CENTER, iconX - 38, iconY + 23);
  lv_obj_set_style_line_color(leftWave, lv_color_hex(iconColor), LV_PART_MAIN);
  lv_obj_set_style_line_width(leftWave, 6, LV_PART_MAIN);
  lv_obj_set_style_line_rounded(leftWave, true, LV_PART_MAIN);

  // Right small wave
  lv_obj_t *rightWave = lv_line_create(contentLayer);
  lv_line_set_points(rightWave, smallWavePts, sizeof(smallWavePts) / sizeof(smallWavePts[0]));
  lv_obj_set_size(rightWave, 40, 8);
  lv_obj_align(rightWave, LV_ALIGN_CENTER, iconX + 34, iconY + 23);
  lv_obj_set_style_line_color(rightWave, lv_color_hex(iconColor), LV_PART_MAIN);
  lv_obj_set_style_line_width(rightWave, 6, LV_PART_MAIN);
  lv_obj_set_style_line_rounded(rightWave, true, LV_PART_MAIN);

  // Bottom wave
  lv_obj_t *bottomWave = lv_line_create(contentLayer);
  lv_line_set_points(bottomWave, bottomWavePts, sizeof(bottomWavePts) / sizeof(bottomWavePts[0]));
  lv_obj_set_size(bottomWave, 80, 10);
  lv_obj_align(bottomWave, LV_ALIGN_CENTER, iconX, iconY + 47);
  lv_obj_set_style_line_color(bottomWave, lv_color_hex(iconColor), LV_PART_MAIN);
  lv_obj_set_style_line_width(bottomWave, 6, LV_PART_MAIN);
  lv_obj_set_style_line_rounded(bottomWave, true, LV_PART_MAIN);
}

void showOilTempGauge()
{
  clearLayer(contentLayer);

  int mode = getCurrentRingColor();
  uint32_t highlight = getRingHighlightColor(mode);

  createLabel(contentLayer, "OIL TEMP", &Xolonium_pn4D32pt7b, highlight, 0, -120);

  int oilTempF = 0;
  int shownTemp = 0;
  const char *degreeSymbol = "\xC2\xB0";
  const char *unitText = oilTempUnitMode == TEMP_UNIT_C ? "C" : "F";

  if (hasLiveOilTemp)
  {
    oilTempF = (int)((liveOilTempC * 9.0 / 5.0) + 32.0 + 0.5);

    if (oilTempUnitMode == TEMP_UNIT_C)
    {
      shownTemp = liveOilTempC;
    }
    else
    {
      shownTemp = oilTempF;
    }
  }

  uint32_t oilColor = getOilTempColor(oilTempF, hasLiveOilTemp);

  char oilText[20];

  if (hasLiveOilTemp)
  {
    snprintf(oilText, sizeof(oilText), "%d%s%s", shownTemp, degreeSymbol, unitText);
  }
  else
  {
    snprintf(oilText, sizeof(oilText), "---%s%s", degreeSymbol, unitText);
  }

  createLabel(contentLayer, oilText, &Xolonium_pn4D72pt7b, oilColor, 0, -40);
  drawOilTempIcon(oilColor);
  drawBluetoothIcon();
}

// =====================================================
// Menu Screens
// =====================================================


void clearLiveData()
{
  liveBatteryVoltage = 0.0;
  hasLiveVoltage = false;

  liveRpm = 0;
  hasLiveRpm = false;

  liveSpeedKph = 0;
  hasLiveSpeed = false;

  liveCoolantTempC = 0;
  hasLiveCoolantTemp = false;

  liveThrottlePercent = 0;
  hasLiveThrottle = false;

  liveIntakeAirTempC = 0;
  hasLiveIntakeAirTemp = false;

  liveMafGps = 0.0;
  hasLiveMaf = false;

  liveOilTempC = 0;
  hasLiveOilTemp = false;
}

// =====================================================
// Brightness Helpers
// =====================================================

int getBrightnessValue()
{
  if (brightnessMode == BRIGHTNESS_DIM)
  {
    return 60;
  }

  if (brightnessMode == BRIGHTNESS_MEDIUM)
  {
    return 150;
  }

  return 255;
}

void applyBrightness()
{
  gfx->setBrightness(getBrightnessValue());
}

void saveBrightnessSettings()
{
  prefs.begin("blueline", false);
  prefs.putInt("brightness", brightnessMode);
  prefs.end();
}

bool isGaugeScreen()
{
  return currentScreen == SCREEN_BATTERY ||
         currentScreen == SCREEN_ALL_IN_ONE ||
         currentScreen == SCREEN_RPM ||
         currentScreen == SCREEN_SPEED ||
         currentScreen == SCREEN_COOLANT ||
         currentScreen == SCREEN_THROTTLE ||
         currentScreen == SCREEN_INTAKE_AIR ||
         currentScreen == SCREEN_AIRFLOW ||
         currentScreen == SCREEN_MPG ||
         currentScreen == SCREEN_CHRONO ||
         currentScreen == SCREEN_TPMS ||
         currentScreen == SCREEN_OIL_TEMP;
}

void cycleBrightness()
{
  brightnessMode++;

  if (brightnessMode > BRIGHTNESS_BRIGHT)
  {
    brightnessMode = BRIGHTNESS_DIM;
  }

  applyBrightness();
  saveBrightnessSettings();

  Serial.print("Brightness mode: ");
  Serial.println(brightnessMode);
}

bool checkDoubleTap(int x, int y)
{
  unsigned long now = millis();

  int dx = abs(x - lastTapX);
  int dy = abs(y - lastTapY);

  bool isDoubleTap = false;

  if (lastTapTime > 0)
  {
    isDoubleTap = (now - lastTapTime <= DOUBLE_TAP_TIME) &&
                  (dx <= DOUBLE_TAP_DISTANCE) &&
                  (dy <= DOUBLE_TAP_DISTANCE);
  }

  if (isDoubleTap)
  {
    // Reset after successful double tap so the next double tap starts fresh
    lastTapTime = 0;
    lastTapX = 0;
    lastTapY = 0;
    return true;
  }

  lastTapTime = now;
  lastTapX = x;
  lastTapY = y;

  return false;
}

// =====================================================
// BLE Scan / Connect
// =====================================================

void forceLvglRefresh()
{
  for (int i = 0; i < 20; i++)
  {
    lv_tick_inc(5);
    lv_timer_handler();
    delay(5);
  }
}

void initBleIfNeeded()
{
  if (!bleStarted)
  {
    BLEDevice::init("BlueLine One");
    bleStarted = true;
    Serial.println("BLE initialized");
  }
}

String shortenBleName(String name, int maxLen)
{
  if (name.length() == 0)
  {
    return "UNKNOWN";
  }

  if (name.length() <= maxLen)
  {
    return name;
  }

  return name.substring(0, maxLen);
}

int scoreBleDevice(BLEAdvertisedDevice device)
{
  int score = 0;

  String name = "UNKNOWN";

  if (device.haveName())
  {
    name = String(device.getName().c_str());
  }

  String lowerName = name;
  lowerName.toLowerCase();

  int rssi = device.getRSSI();

  // =====================================================
  // Name-based scoring
  // =====================================================

  if (lowerName.indexOf("lelink") >= 0) score += 120;
  if (lowerName.indexOf("veepeak") >= 0) score += 110;
  if (lowerName.indexOf("vlink") >= 0) score += 100;
  if (lowerName.indexOf("obdii") >= 0) score += 100;
  if (lowerName.indexOf("obd") >= 0) score += 90;
  if (lowerName.indexOf("elm") >= 0) score += 70;
  if (lowerName.indexOf("car") >= 0) score += 30;

  // =====================================================
  // Service UUID scoring
  // =====================================================

  if (device.haveServiceUUID())
  {
    if (device.isAdvertisingService(OBD_SERVICE_UUID))
    {
      score += 120;
    }
  }

  // =====================================================
  // RSSI signal strength scoring
  // =====================================================

  if (rssi >= -45) score += 50;
  else if (rssi >= -55) score += 40;
  else if (rssi >= -65) score += 30;
  else if (rssi >= -75) score += 20;
  else if (rssi >= -85) score += 10;

  // =====================================================
  // Unknown-name handling
  // =====================================================

  if (!device.haveName())
  {
    // Push random unnamed BLE devices lower
    score -= 25;

    // But if it is very close, keep it competitive
    if (rssi >= -50)
    {
      score += 25;
    }

    // If it advertises the OBD service, it is probably important
    if (device.haveServiceUUID() && device.isAdvertisingService(OBD_SERVICE_UUID))
    {
      score += 50;
    }
  }
  else
  {
    // Named devices are easier to identify on screen
    score += 10;
  }

  return score;
}

void sortBleResults()
{
  for (int i = 0; i < bleResultCount - 1; i++)
  {
    for (int j = i + 1; j < bleResultCount; j++)
    {
      if (bleResults[j].score > bleResults[i].score)
      {
        BleCandidate temp = bleResults[i];
        bleResults[i] = bleResults[j];
        bleResults[j] = temp;
      }
    }
  }
}

void scanBleDevices()
{
  initBleIfNeeded();

  bleResultCount = 0;
  bleStatusText = "SCANNING...";
  bleMenuState = BLE_MENU_MAIN;
  redrawUI();
  forceLvglRefresh();

  Serial.println("Starting BLE scan...");

  BLEScan *scanner = BLEDevice::getScan();
  scanner->setActiveScan(true);
  scanner->setInterval(100);
  scanner->setWindow(80);

  BLEScanResults *foundDevices = scanner->start(5, false);
  int count = foundDevices->getCount();

  Serial.print("BLE devices found: ");
  Serial.println(count);

  for (int i = 0; i < count; i++)
  {
    BLEAdvertisedDevice device = foundDevices->getDevice(i);
    String name = "UNKNOWN";
    if (device.haveName()) name = String(device.getName().c_str());

    String address = String(device.getAddress().toString().c_str());
    int rssi = device.getRSSI();
    int score = scoreBleDevice(device);

    bool isUnknown = name == "UNKNOWN";

    // Hide weak random unnamed BLE devices from the top device list.
    // Strong unknown devices can still appear, and unknown devices that
    // advertise the OBD service will score high enough to stay visible.
    if (isUnknown && score < 35)
    {
      continue;
    }

    Serial.print("Device: "); Serial.print(name);
    Serial.print(" | "); Serial.print(address);
    Serial.print(" | RSSI "); Serial.print(rssi);
    Serial.print(" | Score "); Serial.println(score);

    if (bleResultCount < BLE_MAX_RESULTS)
    {
      bleResults[bleResultCount].name = name;
      bleResults[bleResultCount].address = address;
      bleResults[bleResultCount].rssi = rssi;
      bleResults[bleResultCount].score = score;
      bleResultCount++;
    }
    else
    {
      int lowestIndex = 0;
      for (int j = 1; j < BLE_MAX_RESULTS; j++)
      {
        if (bleResults[j].score < bleResults[lowestIndex].score) lowestIndex = j;
      }
      if (score > bleResults[lowestIndex].score)
      {
        bleResults[lowestIndex].name = name;
        bleResults[lowestIndex].address = address;
        bleResults[lowestIndex].rssi = rssi;
        bleResults[lowestIndex].score = score;
      }
    }
  }

  sortBleResults();
  scanner->clearResults();

  bleMenuState = BLE_MENU_SELECT;
  bleStatusText = bleResultCount == 0 ? "NO DEVICES" : "SELECT DEVICE";
  redrawUI();
}


void disconnectBle()
{
  if (bleClient != nullptr)
  {
    if (bleClient->isConnected())
    {
      bleClient->disconnect();
    }
  }

  obdCharacteristic = nullptr;
  obdNotifyReady = false;
  obdInitDone = false;
  bleConnected = false;
  obdReady = false;
  connectedBleName = "";
  connectedBleAddress = "";
  bleStatusText = "DISCONNECTED";

  clearLiveData();
}

bool connectToBleResult(int index, int failReturnState)
{
  if (index < 0 || index >= bleResultCount) return false;

  initBleIfNeeded();

  clearLiveData();

  String name = bleResults[index].name;
  String address = bleResults[index].address;

  bleConnectingName = shortenBleName(name, 16);
  bleConnectMessage = "CONNECTING";
  bleMenuState = BLE_MENU_CONNECTING;
  redrawUI();
  forceLvglRefresh();

  Serial.print("Connecting to "); Serial.print(name);
  Serial.print(" at "); Serial.println(address);

  if (bleClient != nullptr)
  {
    if (bleClient->isConnected())
    {
      bleClient->disconnect();
      delay(200);
    }
    delete bleClient;
    bleClient = nullptr;
  }

  bleClient = BLEDevice::createClient();
  BLEAddress bleAddress(address.c_str());

  if (!bleClient->connect(bleAddress))
  {
    Serial.println("BLE connect failed");
    bleConnected = false;
    obdReady = false;
    obdCharacteristic = nullptr;
    bleConnectMessage = "FAILED";
    bleStatusText = "FAILED";
    redrawUI();
    forceLvglRefresh();
    delay(1200);

    bleMenuState = failReturnState;
    bleStatusText = (failReturnState == BLE_MENU_SELECT) ? "SELECT DEVICE" : "FAILED";
    redrawUI();
    return false;
  }

  Serial.println("BLE connected");
  bleConnected = true;
  connectedBleName = name;
  connectedBleAddress = address;

  BLERemoteService *remoteService = bleClient->getService(OBD_SERVICE_UUID);
  if (remoteService == nullptr)
  {
    Serial.println("OBD service not found");
    bleConnected = false;
    obdReady = false;
    obdCharacteristic = nullptr;
    bleConnectMessage = "FAILED";
    bleStatusText = "NO OBD SERVICE";
    redrawUI();
    forceLvglRefresh();
    delay(1200);
    if (bleClient != nullptr && bleClient->isConnected()) bleClient->disconnect();

    bleMenuState = failReturnState;
    bleStatusText = (failReturnState == BLE_MENU_SELECT) ? "SELECT DEVICE" : "FAILED";
    redrawUI();
    return false;
  }

  obdCharacteristic = remoteService->getCharacteristic(OBD_CHAR_UUID);
  if (obdCharacteristic == nullptr)
  {
    Serial.println("OBD characteristic not found");
    bleConnected = false;
    obdReady = false;
    obdCharacteristic = nullptr;
    bleConnectMessage = "FAILED";
    bleStatusText = "NO OBD CHAR";
    redrawUI();
    forceLvglRefresh();
    delay(1200);
    if (bleClient != nullptr && bleClient->isConnected()) bleClient->disconnect();

    bleMenuState = failReturnState;
    bleStatusText = (failReturnState == BLE_MENU_SELECT) ? "SELECT DEVICE" : "FAILED";
    redrawUI();
    return false;
  }

  obdReady = true;

  if (!initializeObdAdapter())
  {
    Serial.println("OBD adapter init failed or incomplete");
  }

  bleConnectMessage = "CONNECTED";
  bleStatusText = "CONNECTED";

  prefs.begin("blueline", false);
  prefs.putString("bleName", connectedBleName);
  prefs.putString("bleAddr", connectedBleAddress);
  prefs.end();

  Serial.println("OBD characteristic ready");
  redrawUI();
  forceLvglRefresh();
  delay(700);

  if (lastGaugeScreen != SCREEN_ALL_IN_ONE && lastGaugeScreen != SCREEN_BATTERY && lastGaugeScreen != SCREEN_RPM && lastGaugeScreen != SCREEN_SPEED && lastGaugeScreen != SCREEN_COOLANT && lastGaugeScreen != SCREEN_THROTTLE && lastGaugeScreen != SCREEN_INTAKE_AIR && lastGaugeScreen != SCREEN_AIRFLOW && lastGaugeScreen != SCREEN_MPG && lastGaugeScreen != SCREEN_CHRONO && lastGaugeScreen != SCREEN_TPMS && lastGaugeScreen != SCREEN_OIL_TEMP)
  {
    lastGaugeScreen = SCREEN_ALL_IN_ONE;
  }

  currentScreen = lastGaugeScreen;
  bleMenuState = BLE_MENU_MAIN;
  redrawUI();
  return true;
}

bool connectToSavedBle()
{
  // RECONNECT is only for the last saved BLE device.
  // It does not use the scan results unless the saved device exists.
  if (connectedBleAddress.length() == 0)
  {
    bleConnectingName = "SCAN FIRST";
    bleConnectMessage = "NO SAVED DEVICE";
    bleMenuState = BLE_MENU_CONNECTING;
    redrawUI();
    forceLvglRefresh();
    delay(1200);

    bleMenuState = BLE_MENU_MAIN;
    bleStatusText = "SCAN FIRST";
    redrawUI();
    return false;
  }

  bleResults[0].name = connectedBleName.length() > 0 ? connectedBleName : "SAVED DEVICE";
  bleResults[0].address = connectedBleAddress;
  bleResults[0].rssi = 0;
  bleResults[0].score = 999;
  if (bleResultCount < 1) bleResultCount = 1;

  return connectToBleResult(0, BLE_MENU_MAIN);
}

bool connectToSavedBleQuiet()
{
  // Quiet auto reconnect path.
  // This avoids the BLE menu CONNECTING / FAILED screens during startup.
  if (connectedBleAddress.length() == 0)
  {
    return false;
  }

  initBleIfNeeded();
  clearLiveData();

  String savedName = connectedBleName.length() > 0 ? connectedBleName : "SAVED DEVICE";
  String savedAddress = connectedBleAddress;

  Serial.print("Quiet auto reconnect to ");
  Serial.print(savedName);
  Serial.print(" at ");
  Serial.println(savedAddress);

  if (bleClient != nullptr)
  {
    if (bleClient->isConnected())
    {
      bleClient->disconnect();
      delay(150);
    }

    delete bleClient;
    bleClient = nullptr;
  }

  bleClient = BLEDevice::createClient();
  BLEAddress bleAddress(savedAddress.c_str());

  if (!bleClient->connect(bleAddress))
  {
    Serial.println("Quiet auto reconnect failed");
    bleConnected = false;
    obdReady = false;
    obdCharacteristic = nullptr;
    obdNotifyReady = false;
    obdInitDone = false;

    // Keep the saved device in RAM so manual RECONNECT still works.
    connectedBleName = savedName;
    connectedBleAddress = savedAddress;
    bleStatusText = "DISCONNECTED";

    return false;
  }

  Serial.println("Quiet auto reconnect BLE connected");

  BLERemoteService *remoteService = bleClient->getService(OBD_SERVICE_UUID);
  if (remoteService == nullptr)
  {
    Serial.println("Quiet auto reconnect: OBD service not found");
    if (bleClient != nullptr && bleClient->isConnected()) bleClient->disconnect();

    bleConnected = false;
    obdReady = false;
    obdCharacteristic = nullptr;
    obdNotifyReady = false;
    obdInitDone = false;
    connectedBleName = savedName;
    connectedBleAddress = savedAddress;
    bleStatusText = "DISCONNECTED";

    return false;
  }

  obdCharacteristic = remoteService->getCharacteristic(OBD_CHAR_UUID);
  if (obdCharacteristic == nullptr)
  {
    Serial.println("Quiet auto reconnect: OBD characteristic not found");
    if (bleClient != nullptr && bleClient->isConnected()) bleClient->disconnect();

    bleConnected = false;
    obdReady = false;
    obdCharacteristic = nullptr;
    obdNotifyReady = false;
    obdInitDone = false;
    connectedBleName = savedName;
    connectedBleAddress = savedAddress;
    bleStatusText = "DISCONNECTED";

    return false;
  }

  bleConnected = true;
  obdReady = true;
  connectedBleName = savedName;
  connectedBleAddress = savedAddress;

  if (!initializeObdAdapter())
  {
    Serial.println("Quiet auto reconnect: OBD init failed or incomplete");
  }

  bleStatusText = "CONNECTED";

  Serial.println("Quiet auto reconnect complete");

  if (contentLayer != nullptr &&
      (currentScreen == SCREEN_BATTERY ||
       currentScreen == SCREEN_RPM ||
       currentScreen == SCREEN_SPEED ||
       currentScreen == SCREEN_COOLANT ||
       currentScreen == SCREEN_THROTTLE ||
       currentScreen == SCREEN_INTAKE_AIR ||
       currentScreen == SCREEN_AIRFLOW ||
       currentScreen == SCREEN_MPG ||
       currentScreen == SCREEN_OIL_TEMP ||
       currentScreen == SCREEN_BLE_MENU))
  {
    redrawUI();
  }

  return true;
}

void handleAutoReconnect()
{
  if (!autoReconnectEnabled) return;
  if (autoReconnectInProgress) return;
  if (bleConnected) return;
  if (connectedBleAddress.length() == 0) return;
  if (autoReconnectAttempts >= AUTO_RECONNECT_MAX_ATTEMPTS) return;

  // Do not auto reconnect while the user is inside menus/settings.
  // Manual RECONNECT still works there.
  if (!isGaugeScreen()) return;

  unsigned long now = millis();

  if (bootCompleteTime == 0) return;

  if (now - bootCompleteTime < AUTO_RECONNECT_START_DELAY)
  {
    return;
  }

  if (autoReconnectAttempts > 0 &&
      now - lastAutoReconnectAttemptTime < AUTO_RECONNECT_RETRY_DELAY)
  {
    return;
  }

  autoReconnectInProgress = true;
  autoReconnectAttempts++;
  lastAutoReconnectAttemptTime = now;

  Serial.print("Auto reconnect attempt ");
  Serial.print(autoReconnectAttempts);
  Serial.print(" of ");
  Serial.println(AUTO_RECONNECT_MAX_ATTEMPTS);

  bool success = connectToSavedBleQuiet();

  autoReconnectInProgress = false;

  if (success)
  {
    Serial.println("Auto reconnect succeeded");
    return;
  }

  if (autoReconnectAttempts >= AUTO_RECONNECT_MAX_ATTEMPTS)
  {
    bleStatusText = "DISCONNECTED";
    Serial.println("Auto reconnect stopped after 1 failed attempt");

    if (isGaugeScreen())
    {
      redrawUI();
    }
  }
}


// =====================================================
// OBD Live Data
// =====================================================

void onObdNotify(BLERemoteCharacteristic *characteristic, uint8_t *data, size_t length, bool isNotify)
{
  for (size_t i = 0; i < length; i++)
  {
    char c = (char)data[i];

    if (c != '\0')
    {
      obdRxBuffer += c;
    }
  }
}

void clearObdRxBuffer()
{
  obdRxBuffer = "";
}

bool sendObdCommandRaw(const String &command)
{
  if (!bleConnected || !obdReady || obdCharacteristic == nullptr)
  {
    return false;
  }

  if (bleClient == nullptr || !bleClient->isConnected())
  {
    bleConnected = false;
    obdReady = false;
    obdInitDone = false;
    obdCharacteristic = nullptr;
    clearLiveData();
    return false;
  }

  String out = command;

  if (!out.endsWith("\r"))
  {
    out += "\r";
  }

  obdCharacteristic->writeValue((uint8_t *)out.c_str(), out.length(), false);
  return true;
}

String sendObdCommand(const String &command, unsigned long timeoutMs)
{
  obdCommandAbortedForTouch = false;
  clearObdRxBuffer();

  if (!sendObdCommandRaw(command))
  {
    return "";
  }

  unsigned long start = millis();

  while (millis() - start < timeoutMs)
  {
    // Touch/swipe must win over OBD. If the user starts swiping, abort the
    // current OBD wait instead of blocking screen navigation.
    handleTouch();
    handleRepeatButtonHold();

    if (touchActive || pendingTouchRelease ||
        (millis() - lastUserTouchActivityTime < OBD_TOUCH_PAUSE_TIME))
    {
      obdCommandAbortedForTouch = true;
      return "";
    }

    lv_tick_inc(5);
    lv_timer_handler();
    delay(5);

    if (obdRxBuffer.indexOf(">") >= 0)
    {
      break;
    }
  }

  String response = obdRxBuffer;
  response.trim();

  Serial.print("OBD ");
  Serial.print(command);
  Serial.print(" => ");
  Serial.println(response);

  return response;
}

bool initializeObdAdapter()
{
  if (!bleConnected || obdCharacteristic == nullptr)
  {
    return false;
  }

  obdNotifyReady = false;
  obdInitDone = false;

  if (obdCharacteristic->canNotify())
  {
    obdCharacteristic->registerForNotify(onObdNotify);
    obdNotifyReady = true;
    Serial.println("OBD notify enabled");
  }
  else
  {
    Serial.println("OBD characteristic does not support notify");
  }

  delay(100);

  // Basic ELM327 setup. Shorter timeouts make startup/auto reconnect faster.
  sendObdCommand("ATZ", 600);
  delay(100);
  sendObdCommand("ATE0", 220);   // echo off
  sendObdCommand("ATL0", 220);   // linefeeds off
  sendObdCommand("ATS0", 220);   // spaces off
  sendObdCommand("ATH0", 220);   // headers off
  sendObdCommand("ATAT1", 220);  // adaptive timing on
  sendObdCommand("ATST0A", 220); // shorter ELM timeout
  sendObdCommand("ATSP0", 350);  // automatic protocol

  obdInitDone = true;
  lastObdPollTime = 0;
  obdPollStep = OBD_POLL_VOLTAGE;

  return true;
}

int hexByteToInt(const String &token)
{
  char *endPtr = nullptr;
  return (int)strtol(token.c_str(), &endPtr, 16);
}

void parseBatteryVoltage(const String &response)
{
  String clean = response;

  // Use ASCII codes to avoid broken escaped newline characters in Arduino.
  clean.replace((char)13, ' ');
  clean.replace((char)10, ' ');
  clean.replace('>', ' ');
  clean.trim();

  // Some adapters return "12.4V", others may return just "12.4".
  // Accept the first realistic voltage-looking number in the response.
  bool foundVoltage = false;

  for (int i = 0; i < clean.length(); i++)
  {
    char c = clean.charAt(i);

    if ((c >= '0' && c <= '9') || c == '.')
    {
      int start = i;
      int end = i;

      while (end < clean.length())
      {
        char n = clean.charAt(end);

        if ((n >= '0' && n <= '9') || n == '.')
        {
          end++;
        }
        else
        {
          break;
        }
      }

      String candidate = clean.substring(start, end);
      float voltage = candidate.toFloat();

      if (voltage > 5.0 && voltage < 20.0)
      {
        liveBatteryVoltage = voltage;
        hasLiveVoltage = true;
        foundVoltage = true;
        break;
      }

      i = end;
    }
  }

  if (!foundVoltage)
  {
    hasLiveVoltage = false;
  }
}

void parseControlModuleVoltage(const String &response)
{
  String clean = response;
  clean.toUpperCase();
  clean.replace("\r", " ");
  clean.replace("\n", " ");
  clean.replace(">", " ");
  clean.replace(" ", "");

  int idx = clean.indexOf("4142");

  if (idx < 0 || clean.length() < idx + 8)
  {
    hasLiveVoltage = false;
    return;
  }

  String aText = clean.substring(idx + 4, idx + 6);
  String bText = clean.substring(idx + 6, idx + 8);

  int A = hexByteToInt(aText);
  int B = hexByteToInt(bText);

  float voltage = ((A * 256) + B) / 1000.0;

  if (voltage > 5.0 && voltage < 20.0)
  {
    liveBatteryVoltage = voltage;
    hasLiveVoltage = true;
  }
  else
  {
    hasLiveVoltage = false;
  }
}

void parseRpm(const String &response)
{
  String clean = response;
  clean.toUpperCase();
  clean.replace("\r", " ");
  clean.replace("\n", " ");
  clean.replace(">", " ");
  clean.replace(" ", "");

  int idx = clean.indexOf("410C");

  if (idx < 0 || clean.length() < idx + 8)
  {
    hasLiveRpm = false;
    return;
  }

  String aText = clean.substring(idx + 4, idx + 6);
  String bText = clean.substring(idx + 6, idx + 8);

  int A = hexByteToInt(aText);
  int B = hexByteToInt(bText);

  int rpm = ((A * 256) + B) / 4;

  if (rpm >= 0 && rpm <= 12000)
  {
    liveRpm = rpm;
    hasLiveRpm = true;
  }
  else
  {
    hasLiveRpm = false;
  }
}

void parseSpeed(const String &response)
{
  String clean = response;
  clean.toUpperCase();
  clean.replace("\r", " ");
  clean.replace("\n", " ");
  clean.replace(">", " ");
  clean.replace(" ", "");

  int idx = clean.indexOf("410D");

  if (idx < 0 || clean.length() < idx + 6)
  {
    hasLiveSpeed = false;
    return;
  }

  String aText = clean.substring(idx + 4, idx + 6);
  int speed = hexByteToInt(aText);

  if (speed >= 0 && speed <= 255)
  {
    liveSpeedKph = speed;
    hasLiveSpeed = true;
  }
  else
  {
    hasLiveSpeed = false;
  }
}

void parseCoolantTemp(const String &response)
{
  String clean = response;
  clean.toUpperCase();
  clean.replace("\r", " ");
  clean.replace("\n", " ");
  clean.replace(">", " ");
  clean.replace(" ", "");

  int idx = clean.indexOf("4105");

  if (idx < 0 || clean.length() < idx + 6)
  {
    hasLiveCoolantTemp = false;
    return;
  }

  String aText = clean.substring(idx + 4, idx + 6);
  int A = hexByteToInt(aText);
  int coolantC = A - 40;

  if (coolantC >= -40 && coolantC <= 215)
  {
    liveCoolantTempC = coolantC;
    hasLiveCoolantTemp = true;
  }
  else
  {
    hasLiveCoolantTemp = false;
  }
}

void parseThrottlePosition(const String &response)
{
  String clean = response;
  clean.toUpperCase();
  clean.replace("\r", " ");
  clean.replace("\n", " ");
  clean.replace(">", " ");
  clean.replace(" ", "");

  int idx = clean.indexOf("4111");

  if (idx < 0 || clean.length() < idx + 6)
  {
    hasLiveThrottle = false;
    return;
  }

  String aText = clean.substring(idx + 4, idx + 6);
  int A = hexByteToInt(aText);

  int throttle = (A * 100 + 127) / 255;

  if (throttle >= 0 && throttle <= 100)
  {
    liveThrottlePercent = throttle;
    hasLiveThrottle = true;
  }
  else
  {
    hasLiveThrottle = false;
  }
}

void parseIntakeAirTemp(const String &response)
{
  String clean = response;
  clean.toUpperCase();
  clean.replace("\r", " ");
  clean.replace("\n", " ");
  clean.replace(">", " ");
  clean.replace(" ", "");

  int idx = clean.indexOf("410F");

  if (idx < 0 || clean.length() < idx + 6)
  {
    hasLiveIntakeAirTemp = false;
    return;
  }

  String aText = clean.substring(idx + 4, idx + 6);
  int A = hexByteToInt(aText);
  int intakeAirC = A - 40;

  if (intakeAirC >= -40 && intakeAirC <= 215)
  {
    liveIntakeAirTempC = intakeAirC;
    hasLiveIntakeAirTemp = true;
  }
  else
  {
    hasLiveIntakeAirTemp = false;
  }
}

void parseMafAirflow(const String &response)
{
  String clean = response;
  clean.toUpperCase();
  clean.replace("\r", " ");
  clean.replace("\n", " ");
  clean.replace(">", " ");
  clean.replace(" ", "");

  int idx = clean.indexOf("4110");

  if (idx < 0 || clean.length() < idx + 8)
  {
    hasLiveMaf = false;
    return;
  }

  String aText = clean.substring(idx + 4, idx + 6);
  String bText = clean.substring(idx + 6, idx + 8);

  int A = hexByteToInt(aText);
  int B = hexByteToInt(bText);

  float maf = ((A * 256) + B) / 100.0;

  if (maf >= 0.0 && maf <= 655.35)
  {
    liveMafGps = maf;
    hasLiveMaf = true;
  }
  else
  {
    hasLiveMaf = false;
  }
}

void parseOilTemp(const String &response)
{
  String clean = response;
  clean.toUpperCase();
  clean.replace("\r", " ");
  clean.replace("\n", " ");
  clean.replace(">", " ");
  clean.replace(" ", "");

  int idx = clean.indexOf("415C");

  if (idx < 0 || clean.length() < idx + 6)
  {
    hasLiveOilTemp = false;
    return;
  }

  String aText = clean.substring(idx + 4, idx + 6);
  int A = hexByteToInt(aText);
  int oilTempC = A - 40;

  if (oilTempC >= -40 && oilTempC <= 215)
  {
    liveOilTempC = oilTempC;
    hasLiveOilTemp = true;
  }
  else
  {
    hasLiveOilTemp = false;
  }
}

void parseObdResponse(const String &command, const String &response)
{
  if (response.length() == 0)
  {
    if (command == "ATRV" || command == "0142") hasLiveVoltage = false;
    if (command == "010C") hasLiveRpm = false;
    if (command == "010D") hasLiveSpeed = false;
    if (command == "0105") hasLiveCoolantTemp = false;
    if (command == "0111") hasLiveThrottle = false;
    if (command == "010F") hasLiveIntakeAirTemp = false;
    if (command == "0110") hasLiveMaf = false;
    if (command == "015C") hasLiveOilTemp = false;
    return;
  }

  if (command == "ATRV")
  {
    parseBatteryVoltage(response);
  }
  else if (command == "0142")
  {
    parseControlModuleVoltage(response);
  }
  else if (command == "010C")
  {
    parseRpm(response);
  }
  else if (command == "010D")
  {
    parseSpeed(response);
  }
  else if (command == "0105")
  {
    parseCoolantTemp(response);
  }
  else if (command == "0111")
  {
    parseThrottlePosition(response);
  }
  else if (command == "010F")
  {
    parseIntakeAirTemp(response);
  }
  else if (command == "0110")
  {
    parseMafAirflow(response);
  }
  else if (command == "015C")
  {
    parseOilTemp(response);
  }
}

String getCurrentGaugeObdCommand()
{
  if (currentScreen == SCREEN_BATTERY) return "ATRV";
  if (currentScreen == SCREEN_ALL_IN_ONE)
  {
    switch (allInOnePollStep)
    {
      case 0: return "010C"; // RPM
      case 1: return "010D"; // Speed
      case 2: return "010C"; // RPM
      case 3: return "0111"; // Throttle
      case 4: return "010C"; // RPM
      case 5: return "010F"; // Intake Air
      default: return "010C";
    }
  }
  if (currentScreen == SCREEN_RPM) return "010C";
  if (currentScreen == SCREEN_SPEED) return "010D";
  if (currentScreen == SCREEN_COOLANT) return "0105";
  if (currentScreen == SCREEN_THROTTLE) return "0111";
  if (currentScreen == SCREEN_INTAKE_AIR) return "010F";
  if (currentScreen == SCREEN_AIRFLOW) return "0110";
  if (currentScreen == SCREEN_MPG)
  {
    return mpgPollStep == 0 ? "010D" : "0110";
  }
  if (currentScreen == SCREEN_CHRONO)
  {
    return chronoPollStep == 0 ? "010D" : "0110";
  }
  if (currentScreen == SCREEN_OIL_TEMP) return "015C";

  return "";
}

unsigned long getCurrentGaugePollInterval(const String &command)
{
  // RPM Turbo Mode: poll RPM as fast as the BLE adapter/ECU can reasonably answer.
  if (RPM_TURBO_MODE && currentScreen == SCREEN_RPM && command == "010C")
  {
    return RPM_TURBO_INTERVAL;
  }

  // All-In-One screen uses a short four-item batch cycle.
  if (currentScreen == SCREEN_ALL_IN_ONE)
  {
    return OBD_POLL_INTERVAL;
  }

  // RPM, speed, throttle, and MAF should feel live.
  if (command == "010C" || command == "010D" || command == "0111" || command == "0110")
  {
    return OBD_POLL_INTERVAL;
  }

  // Temps and voltage do not need to update as fast.
  return 350;
}

unsigned long getCurrentGaugeObdTimeout(const String &command)
{
  // ATRV can be slower when the car is off and the adapter is awake by itself.
  // Give it more time so adapter voltage still works without ignition.
  if (command == "ATRV")
  {
    return 600;
  }

  if (command == "0142")
  {
    return 250;
  }

  // RPM Turbo Mode: shorter wait so stale/no-response packets do not lock the RPM gauge.
  // All-In-One also uses this for the RPM-priority batch cycle.
  if (RPM_TURBO_MODE && (currentScreen == SCREEN_RPM || currentScreen == SCREEN_ALL_IN_ONE) && command == "010C")
  {
    return RPM_TURBO_TIMEOUT;
  }

  // All-In-One screen keeps the four-value batch from feeling locked.
  if (currentScreen == SCREEN_ALL_IN_ONE)
  {
    return 110;
  }

  // Fast-changing PIDs use a shorter wait so the UI/touch does not feel locked.
  if (command == "010C" || command == "010D" || command == "0111" || command == "0110")
  {
    return 110;
  }

  return 180;
}

void pollObdLiveData()
{
  if (!bleConnected || !obdReady || !obdInitDone || obdCharacteristic == nullptr)
  {
    return;
  }

  // Do not keep polling OBD while inside menus/settings.
  if (!isGaugeScreen())
  {
    return;
  }

  // Touch/swipe has priority over live data. This prevents the BLE OBD wait
  // loop from making the gauge feel locked while connected.
  unsigned long now = millis();
  if (touchActive || pendingTouchRelease ||
      (now - lastUserTouchActivityTime < OBD_TOUCH_PAUSE_TIME))
  {
    return;
  }

  if (bleClient == nullptr || !bleClient->isConnected())
  {
    Serial.println("BLE disconnected during OBD poll");
    disconnectBle();
    redrawUI();
    return;
  }

  String command = getCurrentGaugeObdCommand();

  if (command.length() == 0)
  {
    return;
  }

  unsigned long interval = getCurrentGaugePollInterval(command);

  if (now - lastObdPollTime < interval)
  {
    return;
  }

  lastObdPollTime = now;

  String response = sendObdCommand(command, getCurrentGaugeObdTimeout(command));

  if (obdCommandAbortedForTouch)
  {
    return;
  }

  parseObdResponse(command, response);

  if (currentScreen == SCREEN_MPG)
  {
    updateAverageMpg();
  }

  // Battery fallback: some BLE OBD adapters do not answer ATRV reliably.
  // If ATRV fails, try standard PID 0142 control module voltage.
  if (currentScreen == SCREEN_BATTERY && command == "ATRV" && !hasLiveVoltage)
  {
    String fallbackResponse = sendObdCommand("0142", getCurrentGaugeObdTimeout("0142"));

    if (!obdCommandAbortedForTouch)
    {
      parseObdResponse("0142", fallbackResponse);
    }
  }

  if (currentScreen == SCREEN_MPG)
  {
    mpgPollStep = mpgPollStep == 0 ? 1 : 0;
  }

  if (currentScreen == SCREEN_ALL_IN_ONE)
  {
    allInOnePollStep++;
    if (allInOnePollStep > 5)
    {
      allInOnePollStep = 0;
    }
  }

  if (currentScreen == SCREEN_CHRONO)
  {
    chronoPollStep = chronoPollStep == 0 ? 1 : 0;
  }

  redrawUI();
}

void showBleMenu()
{
  clearLayer(contentLayer);

  int mode = getCurrentRingColor();
  uint32_t main = getRingMainColor(mode);
  uint32_t highlight = getRingHighlightColor(mode);
  uint32_t inner = getRingInnerColor(mode);

  createLabel(contentLayer, "BLE MENU", &Xolonium_pn4D24pt7b, highlight, 0, -135);

  if (bleMenuState == BLE_MENU_CONNECTING)
  {
    uint32_t statusColor = 0xFFAA00;
    if (bleConnectMessage == "CONNECTED") statusColor = 0x00FF80;
    else if (bleConnectMessage == "FAILED") statusColor = 0xFF0000;

    createLabel(contentLayer, bleConnectMessage.c_str(), &Xolonium_pn4D32pt7b, statusColor, 0, -40);
    if (bleConnectingName.length() > 0)
    {
      createLabel(contentLayer, bleConnectingName.c_str(), &Xolonium_pn4D18pt7b, 0xFFFFFF, 0, 10);
    }
    return;
  }

  if (bleMenuState == BLE_MENU_SELECT)
  {
    createLabel(contentLayer, "SELECT DEVICE", &Xolonium_pn4D18pt7b, 0xFFFFFF, 0, -98);

    // Scan results use a 2x2 circle layout:
    // 1 / 2 on the top row, 3 / BACK on the bottom row.
    const int circleSize = 76;
    const int deviceX[3] = {150, 316, 150};
    const int deviceY[3] = {195, 195, 315};

    if (bleResultCount == 0)
    {
      createLabel(contentLayer, "NO DEVICES", &Xolonium_pn4D24pt7b, 0xFFAA00, 0, -20);
    }
    else
    {
      for (int i = 0; i < bleResultCount && i < 3; i++)
      {
        char numberText[4];
        snprintf(numberText, sizeof(numberText), "%d", i + 1);

        String shortName = shortenBleName(bleResults[i].name, 9);

        createCircleButtonCustom(
          contentLayer,
          numberText,
          shortName.c_str(),
          deviceX[i],
          deviceY[i],
          circleSize,
          56,
          &Xolonium_pn4D24pt7b,
          &Xolonium_pn4D18pt7b,
          highlight,
          highlight,
          0xFFFFFF
        );
      }
    }

    // Fourth circle is BACK in the bottom-right position.
    createCircleButtonCustom(
      contentLayer,
      "B",
      "BACK",
      316,
      315,
      circleSize,
      56,
      &Xolonium_pn4D24pt7b,
      &Xolonium_pn4D18pt7b,
      inner,
      inner,
      0xFFFFFF
    );
    return;
  }

  if (bleConnected && obdReady)
  {
    createLabel(contentLayer, "CONNECTED", &Xolonium_pn4D24pt7b, 0x00FF80, 0, -95);
  }
  else if (bleConnected)
  {
    createLabel(contentLayer, "CONNECTED", &Xolonium_pn4D24pt7b, 0x00FF80, 0, -95);
  }
  else
  {
    createLabel(contentLayer, bleStatusText.c_str(), &Xolonium_pn4D18pt7b, 0xFFAA00, 0, -95);
  }

  // Adapter name intentionally hidden on the main BLE menu.

  // 2x2 circular BLE menu
  createCircleMenuButton(contentLayer, "S", "SCAN",      150, 195, main,      main,      0xFFFFFF);
  createCircleMenuButton(contentLayer, "R", "RECONNECT", 316, 195, highlight, highlight, 0xFFFFFF);
  createCircleMenuButton(contentLayer, "F", "FORGET",    150, 315, 0xFF0000,  0xFF0000,  0xFFFFFF);
  createCircleMenuButton(contentLayer, "B", "BACK",      316, 315, inner,     inner,     0xFFFFFF);
}



void showShiftSettings()
{
  clearLayer(contentLayer);

  int mode = getCurrentRingColor();
  uint32_t main = getRingMainColor(mode);
  uint32_t highlight = getRingHighlightColor(mode);
  uint32_t inner = getRingInnerColor(mode);

  createLabel(contentLayer, "SHIFT SETUP", &Xolonium_pn4D24pt7b, highlight, 0, -135);

  char stepText[20];
  snprintf(stepText, sizeof(stepText), "STEP %d", selectedShiftStep);

  createLabel(contentLayer, stepText, &Xolonium_pn4D24pt7b, main, 0, -105);

  char rpmText[24];
  snprintf(rpmText, sizeof(rpmText), "%d RPM", shiftLightRpm[selectedShiftStep]);

  createLabel(contentLayer, rpmText, &Xolonium_pn4D32pt7b, getShiftLightColor(selectedShiftStep), 0, -60);

  drawShiftLights(shiftLightRpm[selectedShiftStep], true);

  // 3x2 circular shift setup menu
  // Top row: LESS / NEXT / MORE
  // Bottom row: SAVE / PREV / BACK
  const int shiftCircleSize = 68;
  const int shiftLabelOffsetY = 50;

  createCircleButtonCustom(
    contentLayer,
    "-",
    "LESS",
    120,
    225,
    shiftCircleSize,
    shiftLabelOffsetY,
    &Xolonium_pn4D24pt7b,
    &Xolonium_pn4D18pt7b,
    0x777777,
    0xAAAAAA,
    0xFFFFFF
  );

  createCircleButtonCustom(
    contentLayer,
    "N",
    "NEXT",
    233,
    225,
    shiftCircleSize,
    shiftLabelOffsetY,
    &Xolonium_pn4D24pt7b,
    &Xolonium_pn4D18pt7b,
    highlight,
    highlight,
    0xFFFFFF
  );

  createCircleButtonCustom(
    contentLayer,
    "+",
    "MORE",
    346,
    225,
    shiftCircleSize,
    shiftLabelOffsetY,
    &Xolonium_pn4D24pt7b,
    &Xolonium_pn4D18pt7b,
    main,
    main,
    0xFFFFFF
  );

  createCircleButtonCustom(
    contentLayer,
    "S",
    "SAVE",
    120,
    330,
    shiftCircleSize,
    shiftLabelOffsetY,
    &Xolonium_pn4D24pt7b,
    &Xolonium_pn4D18pt7b,
    0x00FF80,
    0x00FF80,
    0xFFFFFF
  );

  createCircleButtonCustom(
    contentLayer,
    "P",
    "PREV",
    233,
    330,
    shiftCircleSize,
    shiftLabelOffsetY,
    &Xolonium_pn4D24pt7b,
    &Xolonium_pn4D18pt7b,
    inner,
    inner,
    0xFFFFFF
  );

  createCircleButtonCustom(
    contentLayer,
    "B",
    "BACK",
    346,
    330,
    shiftCircleSize,
    shiftLabelOffsetY,
    &Xolonium_pn4D24pt7b,
    &Xolonium_pn4D18pt7b,
    0x777777,
    0xAAAAAA,
    0xFFFFFF
  );
}

// =====================================================
// Draw Dispatcher
// =====================================================

void drawCurrentScreen()
{
  switch (currentScreen)
  {
    case SCREEN_ALL_IN_ONE:
      showAllInOneGauge();
      break;

    case SCREEN_RPM:
      showRpmGauge();
      break;

    case SCREEN_SPEED:
      showSpeedGauge();
      break;

    case SCREEN_COOLANT:
      showCoolantGauge();
      break;

    case SCREEN_THROTTLE:
      showThrottleGauge();
      break;

    case SCREEN_INTAKE_AIR:
      showIntakeAirGauge();
      break;

    case SCREEN_AIRFLOW:
      showAirflowGauge();
      break;

    case SCREEN_MPG:
      showMpgGauge();
      break;

    case SCREEN_CHRONO:
      showChronographGauge();
      break;

    case SCREEN_TPMS:
      showTpmsGauge();
      break;

    case SCREEN_OIL_TEMP:
      showOilTempGauge();
      break;

    case SCREEN_BLE_MENU:
      showBleMenu();
      break;

    case SCREEN_SHIFT_SETTINGS:
      showShiftSettings();
      break;

    case SCREEN_BATTERY:
    default:
      showBatteryGauge();
      break;
  }
}

void redrawUI()
{
  drawRings();
  drawCurrentScreen();
}

// =====================================================
// Navigation
// =====================================================

void nextGauge()
{
  if (currentScreen == SCREEN_ALL_IN_ONE)
  {
    currentScreen = SCREEN_RPM;
  }
  else if (currentScreen == SCREEN_RPM)
  {
    currentScreen = SCREEN_SPEED;
  }
  else if (currentScreen == SCREEN_SPEED)
  {
    currentScreen = SCREEN_THROTTLE;
  }
  else if (currentScreen == SCREEN_THROTTLE)
  {
    currentScreen = SCREEN_INTAKE_AIR;
  }
  else if (currentScreen == SCREEN_INTAKE_AIR)
  {
    currentScreen = SCREEN_COOLANT;
  }
  else if (currentScreen == SCREEN_COOLANT)
  {
    currentScreen = SCREEN_BATTERY;
  }
  else if (currentScreen == SCREEN_BATTERY)
  {
    currentScreen = SCREEN_TPMS;
  }
  else if (currentScreen == SCREEN_TPMS)
  {
    currentScreen = SCREEN_AIRFLOW;
  }
  else if (currentScreen == SCREEN_AIRFLOW)
  {
    currentScreen = SCREEN_MPG;
    mpgPollStep = 0;
  }
  else if (currentScreen == SCREEN_MPG)
  {
    currentScreen = SCREEN_CHRONO;
    chronoPollStep = 0;
  }
  else
  {
    currentScreen = SCREEN_ALL_IN_ONE;
    allInOnePollStep = 0;
  }

  lastGaugeScreen = currentScreen;
  lastObdPollTime = 0;
  redrawUI();
}

void previousGauge()
{
  if (currentScreen == SCREEN_ALL_IN_ONE)
  {
    currentScreen = SCREEN_CHRONO;
    chronoPollStep = 0;
  }
  else if (currentScreen == SCREEN_CHRONO)
  {
    currentScreen = SCREEN_MPG;
    mpgPollStep = 0;
  }
  else if (currentScreen == SCREEN_MPG)
  {
    currentScreen = SCREEN_AIRFLOW;
  }
  else if (currentScreen == SCREEN_AIRFLOW)
  {
    currentScreen = SCREEN_TPMS;
  }
  else if (currentScreen == SCREEN_TPMS)
  {
    currentScreen = SCREEN_BATTERY;
  }
  else if (currentScreen == SCREEN_BATTERY)
  {
    currentScreen = SCREEN_COOLANT;
  }
  else if (currentScreen == SCREEN_COOLANT)
  {
    currentScreen = SCREEN_INTAKE_AIR;
  }
  else if (currentScreen == SCREEN_INTAKE_AIR)
  {
    currentScreen = SCREEN_THROTTLE;
  }
  else if (currentScreen == SCREEN_THROTTLE)
  {
    currentScreen = SCREEN_SPEED;
  }
  else if (currentScreen == SCREEN_SPEED)
  {
    currentScreen = SCREEN_RPM;
  }
  else if (currentScreen == SCREEN_RPM)
  {
    currentScreen = SCREEN_ALL_IN_ONE;
    allInOnePollStep = 0;
  }
  else
  {
    currentScreen = SCREEN_ALL_IN_ONE;
    allInOnePollStep = 0;
  }

  lastGaugeScreen = currentScreen;
  lastObdPollTime = 0;
  redrawUI();
}

void nextRingColor()
{
  int currentColor = getCurrentRingColor();
  int newColor = wrapRingColor(currentColor + 1);

  setCurrentRingColor(newColor);
  redrawUI();
}

void previousRingColor()
{
  int currentColor = getCurrentRingColor();
  int newColor = wrapRingColor(currentColor - 1);

  setCurrentRingColor(newColor);
  redrawUI();
}

// =====================================================
// Shift RPM Adjust
// =====================================================

void adjustSelectedShiftRpm(int amount)
{
  shiftLightRpm[selectedShiftStep] += amount;

  if (shiftLightRpm[selectedShiftStep] < SHIFT_RPM_MIN)
  {
    shiftLightRpm[selectedShiftStep] = SHIFT_RPM_MIN;
  }

  if (shiftLightRpm[selectedShiftStep] > SHIFT_RPM_MAX)
  {
    shiftLightRpm[selectedShiftStep] = SHIFT_RPM_MAX;
  }

  redrawUI();
}

// =====================================================
// Preferences
// =====================================================

void loadSettings()
{
  prefs.begin("blueline", true);

  batteryRingColor = prefs.getInt("batteryRing", RING_CYAN);
  allInOneRingColor = prefs.getInt("allRing", RING_CYAN);
  rpmRingColor = prefs.getInt("rpmRing", RING_CYAN);
  speedRingColor = prefs.getInt("speedRing", RING_CYAN);
  coolantRingColor = prefs.getInt("coolantRing", RING_CYAN);
  throttleRingColor = prefs.getInt("throttleRing", RING_CYAN);
  intakeAirRingColor = prefs.getInt("intakeRing", RING_CYAN);
  airflowRingColor = prefs.getInt("airflowRing", RING_CYAN);
  mpgRingColor = prefs.getInt("mpgRing", RING_CYAN);
  chronoRingColor = prefs.getInt("chronoRing", RING_BLUE);
  tpmsRingColor = prefs.getInt("tpmsRing", RING_CYAN);
  oilTempRingColor = prefs.getInt("oilRing", RING_CYAN);
  speedUnitMode = prefs.getInt("speedUnit", SPEED_UNIT_MPH);
  coolantUnitMode = prefs.getInt("coolantUnit", TEMP_UNIT_F);
  intakeAirUnitMode = prefs.getInt("intakeUnit", TEMP_UNIT_F);
  oilTempUnitMode = prefs.getInt("oilUnit", TEMP_UNIT_F);
  tpmsUnitMode = prefs.getInt("tpmsUnit", TPMS_UNIT_PSI);
  brightnessMode = prefs.getInt("brightness", BRIGHTNESS_BRIGHT);
  connectedBleName = prefs.getString("bleName", "");
  connectedBleAddress = prefs.getString("bleAddr", "");
  mpgTripMiles = prefs.getFloat("mpgMiles", 0.0);
  mpgTripGallons = prefs.getFloat("mpgGallons", 0.0);

  prefs.end();

  if (mpgTripMiles < 0.0 || mpgTripMiles > 100000.0)
  {
    mpgTripMiles = 0.0;
  }

  if (mpgTripGallons < 0.0 || mpgTripGallons > 10000.0)
  {
    mpgTripGallons = 0.0;
  }

  lastMpgAverageUpdateTime = millis();
  lastMpgAverageSaveTime = millis();

  batteryRingColor = wrapRingColor(batteryRingColor);
  allInOneRingColor = wrapRingColor(allInOneRingColor);
  rpmRingColor = wrapRingColor(rpmRingColor);
  speedRingColor = wrapRingColor(speedRingColor);
  coolantRingColor = wrapRingColor(coolantRingColor);
  throttleRingColor = wrapRingColor(throttleRingColor);
  intakeAirRingColor = wrapRingColor(intakeAirRingColor);
  airflowRingColor = wrapRingColor(airflowRingColor);
  mpgRingColor = wrapRingColor(mpgRingColor);
  chronoRingColor = wrapRingColor(chronoRingColor);
  tpmsRingColor = wrapRingColor(tpmsRingColor);
  oilTempRingColor = wrapRingColor(oilTempRingColor);

  if (tpmsUnitMode != TPMS_UNIT_PSI && tpmsUnitMode != TPMS_UNIT_KPA)
  {
    tpmsUnitMode = TPMS_UNIT_PSI;
  }

  if (speedUnitMode != SPEED_UNIT_MPH && speedUnitMode != SPEED_UNIT_KPH)
  {
    speedUnitMode = SPEED_UNIT_MPH;
  }

  if (coolantUnitMode != TEMP_UNIT_F && coolantUnitMode != TEMP_UNIT_C)
  {
    coolantUnitMode = TEMP_UNIT_F;
  }

  if (intakeAirUnitMode != TEMP_UNIT_F && intakeAirUnitMode != TEMP_UNIT_C)
  {
    intakeAirUnitMode = TEMP_UNIT_F;
  }

  if (oilTempUnitMode != TEMP_UNIT_F && oilTempUnitMode != TEMP_UNIT_C)
  {
    oilTempUnitMode = TEMP_UNIT_F;
  }

  if (brightnessMode < BRIGHTNESS_DIM || brightnessMode > BRIGHTNESS_BRIGHT)
  {
    brightnessMode = BRIGHTNESS_BRIGHT;
  }

  if (connectedBleAddress.length() > 0)
  {
    bleStatusText = "SAVED DEVICE";
  }

  loadShiftLightSettings();
}

void saveRingSettings()
{
  prefs.begin("blueline", false);

  prefs.putInt("batteryRing", batteryRingColor);
  prefs.putInt("allRing", allInOneRingColor);
  prefs.putInt("rpmRing", rpmRingColor);
  prefs.putInt("speedRing", speedRingColor);
  prefs.putInt("coolantRing", coolantRingColor);
  prefs.putInt("throttleRing", throttleRingColor);
  prefs.putInt("intakeRing", intakeAirRingColor);
  prefs.putInt("airflowRing", airflowRingColor);
  prefs.putInt("mpgRing", mpgRingColor);
  prefs.putInt("chronoRing", chronoRingColor);
  prefs.putInt("tpmsRing", tpmsRingColor);
  prefs.putInt("oilRing", oilTempRingColor);

  prefs.end();
}

void saveSpeedSettings()
{
  prefs.begin("blueline", false);
  prefs.putInt("speedUnit", speedUnitMode);
  prefs.end();
}

void saveCoolantUnitSettings()
{
  prefs.begin("blueline", false);
  prefs.putInt("coolantUnit", coolantUnitMode);
  prefs.end();
}

void saveIntakeAirUnitSettings()
{
  prefs.begin("blueline", false);
  prefs.putInt("intakeUnit", intakeAirUnitMode);
  prefs.end();
}

void saveOilTempUnitSettings()
{
  prefs.begin("blueline", false);
  prefs.putInt("oilUnit", oilTempUnitMode);
  prefs.end();
}

void saveTpmsUnitSettings()
{
  prefs.begin("blueline", false);
  prefs.putInt("tpmsUnit", tpmsUnitMode);
  prefs.end();
}

void saveMpgAverageSettings()
{
  prefs.begin("blueline", false);
  prefs.putFloat("mpgMiles", mpgTripMiles);
  prefs.putFloat("mpgGallons", mpgTripGallons);
  prefs.end();
}

void loadShiftLightSettings()
{
  prefs.begin("blueline", true);

  for (int i = 0; i < SHIFT_STEP_COUNT; i++)
  {
    String key = "shift" + String(i);
    shiftLightRpm[i] = prefs.getInt(key.c_str(), shiftLightRpm[i]);

    if (shiftLightRpm[i] < SHIFT_RPM_MIN) shiftLightRpm[i] = SHIFT_RPM_MIN;
    if (shiftLightRpm[i] > SHIFT_RPM_MAX) shiftLightRpm[i] = SHIFT_RPM_MAX;
  }

  prefs.end();
}

void saveShiftLightSettings()
{
  prefs.begin("blueline", false);

  for (int i = 0; i < SHIFT_STEP_COUNT; i++)
  {
    String key = "shift" + String(i);
    prefs.putInt(key.c_str(), shiftLightRpm[i]);
  }

  prefs.end();
}

// =====================================================
// Touch
// =====================================================

void setupTouch()
{
  pinMode(TP_INT, INPUT);

  touch.setPins(TP_RESET, TP_INT);

  if (!touch.begin(Wire, CST92XX_SLAVE_ADDRESS, IIC_SDA, IIC_SCL))
  {
    Serial.println("Touch begin failed");
    touchReady = false;
  }
  else
  {
    touchReady = true;

    Serial.print("Touch ready: ");
    Serial.println(touch.getModelName());

    touch.setResolution(LCD_WIDTH, LCD_HEIGHT);
    touch.setTargetResolution(LCD_WIDTH, LCD_HEIGHT);
  }
}

bool readTouchPoint(int &x, int &y)
{
  if (!touchReady)
  {
    return false;
  }

  const TouchPoints &points = touch.getTouchPoints();

  if (!points.hasPoints())
  {
    return false;
  }

  const TouchPoint &p = points.getPoint(0);

  int rawX = p.x;
  int rawY = p.y;

  if (TOUCH_SWAP_XY)
  {
    int temp = rawX;
    rawX = rawY;
    rawY = temp;
  }

  if (TOUCH_FLIP_X)
  {
    rawX = LCD_WIDTH - rawX;
  }

  if (TOUCH_FLIP_Y)
  {
    rawY = LCD_HEIGHT - rawY;
  }

  x = rawX;
  y = rawY;

  return true;
}

// =====================================================
// Long Press Target System
// =====================================================

int getLongPressTargetFromStart(int x, int y)
{
  if (currentScreen == SCREEN_ALL_IN_ONE)
  {
    // All-In-One BT icon is at the bottom center.
    if (isInRect(x, y, 110, 315, 246, 151))
    {
      Serial.println("Long press target = BT");
      return LONG_TARGET_BT;
    }

    // All-In-One speed number + MPH/KPH area.
    if (isInRect(x, y, 92, 145, 282, 100))
    {
      Serial.println("Long press target = UNIT");
      return LONG_TARGET_UNIT;
    }

    // All-In-One intake air area on the lower-right side.
    if (isInRect(x, y, 235, 250, 210, 145))
    {
      Serial.println("Long press target = INTAKE AIR UNIT");
      return LONG_TARGET_INTAKE_AIR_UNIT;
    }
  }

  if (currentScreen == SCREEN_RPM)
  {
    if (isInRect(x, y, 110, 230, 246, 120))
    {
      Serial.println("Long press target = GEAR");
      return LONG_TARGET_GEAR;
    }
  }

  if (currentScreen == SCREEN_SPEED)
  {
    if (isInRect(x, y, 90, 220, 286, 150))
    {
      Serial.println("Long press target = UNIT");
      return LONG_TARGET_UNIT;
    }
  }

  if (currentScreen == SCREEN_COOLANT)
  {
    if (isInRect(x, y, 80, 145, 306, 145))
    {
      Serial.println("Long press target = COOLANT UNIT");
      return LONG_TARGET_COOLANT_UNIT;
    }
  }

  if (currentScreen == SCREEN_INTAKE_AIR)
  {
    if (isInRect(x, y, 80, 145, 306, 145))
    {
      Serial.println("Long press target = INTAKE AIR UNIT");
      return LONG_TARGET_INTAKE_AIR_UNIT;
    }
  }

  if (currentScreen == SCREEN_OIL_TEMP)
  {
    if (isInRect(x, y, 80, 145, 306, 145))
    {
      Serial.println("Long press target = OIL TEMP UNIT");
      return LONG_TARGET_OIL_UNIT;
    }
  }

  if (currentScreen == SCREEN_MPG)
  {
    if (isInRect(x, y, 80, 145, 306, 145))
    {
      Serial.println("Long press target = MPG RESET");
      return LONG_TARGET_MPG_RESET;
    }
  }

  if (currentScreen == SCREEN_TPMS)
  {
    if (isInRect(x, y, 130, 175, 206, 116))
    {
      Serial.println("Long press target = TPMS UNIT");
      return LONG_TARGET_TPMS_UNIT;
    }
  }

  if (currentScreen == SCREEN_BATTERY ||
      currentScreen == SCREEN_ALL_IN_ONE ||
      currentScreen == SCREEN_RPM ||
      currentScreen == SCREEN_SPEED ||
      currentScreen == SCREEN_COOLANT ||
      currentScreen == SCREEN_THROTTLE ||
      currentScreen == SCREEN_INTAKE_AIR ||
      currentScreen == SCREEN_AIRFLOW ||
      currentScreen == SCREEN_MPG ||
      currentScreen == SCREEN_CHRONO ||
      currentScreen == SCREEN_TPMS ||
      currentScreen == SCREEN_OIL_TEMP)
  {
    if (isInRect(x, y, 110, 315, 246, 151))
    {
      Serial.println("Long press target = BT");
      return LONG_TARGET_BT;
    }
  }

  Serial.println("Long press target = NONE");
  return LONG_TARGET_NONE;
}

void triggerLongPressTarget()
{
  if (longPressTarget == LONG_TARGET_BT)
  {
    Serial.println("BT long press triggered");
    openBleMenu();
    return;
  }

  if (longPressTarget == LONG_TARGET_GEAR)
  {
    Serial.println("Gear long press triggered");
    lastGaugeScreen = SCREEN_RPM;
    currentScreen = SCREEN_SHIFT_SETTINGS;
    redrawUI();
    return;
  }

  if (longPressTarget == LONG_TARGET_UNIT)
  {
    Serial.println("Unit long press triggered");

    speedUnitMode = speedUnitMode == SPEED_UNIT_MPH ? SPEED_UNIT_KPH : SPEED_UNIT_MPH;
    saveSpeedSettings();
    redrawUI();
    return;
  }

  if (longPressTarget == LONG_TARGET_COOLANT_UNIT)
  {
    Serial.println("Coolant unit long press triggered");

    coolantUnitMode = coolantUnitMode == TEMP_UNIT_F ? TEMP_UNIT_C : TEMP_UNIT_F;
    saveCoolantUnitSettings();
    redrawUI();
    return;
  }

  if (longPressTarget == LONG_TARGET_INTAKE_AIR_UNIT)
  {
    Serial.println("Intake air unit long press triggered");

    intakeAirUnitMode = intakeAirUnitMode == TEMP_UNIT_F ? TEMP_UNIT_C : TEMP_UNIT_F;
    saveIntakeAirUnitSettings();
    redrawUI();
    return;
  }

  if (longPressTarget == LONG_TARGET_OIL_UNIT)
  {
    Serial.println("Oil temp unit long press triggered");

    oilTempUnitMode = oilTempUnitMode == TEMP_UNIT_F ? TEMP_UNIT_C : TEMP_UNIT_F;
    saveOilTempUnitSettings();
    redrawUI();
    return;
  }

  if (longPressTarget == LONG_TARGET_MPG_RESET)
  {
    Serial.println("MPG average reset long press triggered");
    resetAverageMpg();
    return;
  }

  if (longPressTarget == LONG_TARGET_TPMS_UNIT)
  {
    Serial.println("TPMS unit long press triggered");

    tpmsUnitMode = tpmsUnitMode == TPMS_UNIT_PSI ? TPMS_UNIT_KPA : TPMS_UNIT_PSI;
    saveTpmsUnitSettings();
    redrawUI();
    return;
  }

  Serial.println("No long press target to trigger");
}

// =====================================================
// Repeat Hold for Shift + / -
// =====================================================

void handleRepeatButtonHold()
{
  if (!touchActive)
  {
    repeatButtonActive = false;
    repeatButtonType = REPEAT_NONE;
    return;
  }

  if (currentScreen != SCREEN_SHIFT_SETTINGS)
  {
    repeatButtonActive = false;
    repeatButtonType = REPEAT_NONE;
    return;
  }

  if (repeatButtonType == REPEAT_NONE)
  {
    return;
  }

  unsigned long now = millis();

  if (!repeatButtonActive)
  {
    if (now - touchStartTime >= REPEAT_BUTTON_DELAY)
    {
      repeatButtonActive = true;
      repeatButtonLastTime = now;

      if (repeatButtonType == REPEAT_MINUS)
      {
        Serial.println("Hold minus: -250");
        adjustSelectedShiftRpm(-SHIFT_RPM_HOLD_STEP);
      }
      else if (repeatButtonType == REPEAT_PLUS)
      {
        Serial.println("Hold plus: +250");
        adjustSelectedShiftRpm(SHIFT_RPM_HOLD_STEP);
      }
    }

    return;
  }

  if (now - repeatButtonLastTime >= REPEAT_BUTTON_INTERVAL)
  {
    repeatButtonLastTime = now;

    if (repeatButtonType == REPEAT_MINUS)
    {
      Serial.println("Repeat minus: -250");
      adjustSelectedShiftRpm(-SHIFT_RPM_HOLD_STEP);
    }
    else if (repeatButtonType == REPEAT_PLUS)
    {
      Serial.println("Repeat plus: +250");
      adjustSelectedShiftRpm(SHIFT_RPM_HOLD_STEP);
    }
  }
}

// =====================================================
// Tap / Swipe / Touch
// =====================================================

void handleTap(int x, int y)
{
  Serial.print("Tap x=");
  Serial.print(x);
  Serial.print(" y=");
  Serial.println(y);

  if (currentScreen == SCREEN_CHRONO)
  {
    // Chronograph touch control:
    // tap anywhere on the screen = reset.
    resetChronograph();
    return;
  }

  if (isGaugeScreen())
  {
    if (checkDoubleTap(x, y))
    {
      cycleBrightness();
      return;
    }
  }

  if (currentScreen == SCREEN_BLE_MENU)
  {
    if (bleMenuState == BLE_MENU_CONNECTING) return;

    if (bleMenuState == BLE_MENU_SELECT)
    {
      const int deviceX[3] = {150, 316, 150};
      const int deviceY[3] = {195, 195, 315};

      for (int i = 0; i < bleResultCount && i < 3; i++)
      {
        // Hitbox includes the circle and the device name underneath.
        if (isInRect(x, y, deviceX[i] - 48, deviceY[i] - 42, 96, 112))
        {
          connectToBleResult(i);
          return;
        }
      }

      // Fourth circle is BACK in the bottom-right position.
      if (isInRect(x, y, 268, 273, 96, 112))
      {
        bleMenuState = BLE_MENU_MAIN;
        bleStatusText = bleConnected ? "CONNECTED" : "READY";
        redrawUI();
        return;
      }

      return;
    }

    // 2x2 circular BLE menu tap zones.
    // The hitboxes include each circle and its label below it.
    if (isInRect(x, y, 104, 157, 92, 108))
    {
      scanBleDevices();
      return;
    }

    if (isInRect(x, y, 270, 157, 92, 108))
    {
      connectToSavedBle();
      return;
    }

    if (isInRect(x, y, 104, 277, 92, 108))
    {
      // FORGET only makes sense if there is a saved BLE device.
      // If nothing is saved, show the same style message as RECONNECT.
      if (connectedBleAddress.length() == 0)
      {
        bleConnectingName = "SCAN FIRST";
        bleConnectMessage = "NO SAVED DEVICE";
        bleMenuState = BLE_MENU_CONNECTING;
        redrawUI();
        forceLvglRefresh();
        delay(1200);

        bleMenuState = BLE_MENU_MAIN;
        bleStatusText = "SCAN FIRST";
        redrawUI();
        return;
      }

      prefs.begin("blueline", false);
      prefs.remove("bleName");
      prefs.remove("bleAddr");
      prefs.end();

      disconnectBle();
      bleResultCount = 0;
      bleMenuState = BLE_MENU_MAIN;

      // After forgetting the saved device, return the main BLE status
      // to the normal connected/disconnected state instead of leaving
      // a temporary DEVICE REMOVED message on the menu.
      bleStatusText = bleConnected ? "CONNECTED" : "DISCONNECTED";

      redrawUI();
      return;
    }

    if (isInRect(x, y, 270, 277, 92, 108))
    {
      bleMenuState = BLE_MENU_MAIN;
      goBackToLastGauge();
      return;
    }
  }

  if (currentScreen == SCREEN_SHIFT_SETTINGS)
  {
    // Top-left: LESS / minus
    if (isInRect(x, y, 76, 188, 88, 92))
    {
      adjustSelectedShiftRpm(-SHIFT_RPM_STEP);
      return;
    }

    // Top-center: NEXT step
    if (isInRect(x, y, 189, 188, 88, 92))
    {
      selectedShiftStep++;

      if (selectedShiftStep >= SHIFT_STEP_COUNT)
      {
        selectedShiftStep = 0;
      }

      redrawUI();
      return;
    }

    // Top-right: MORE / plus
    if (isInRect(x, y, 302, 188, 88, 92))
    {
      adjustSelectedShiftRpm(SHIFT_RPM_STEP);
      return;
    }

    // Bottom-left: SAVE
    if (isInRect(x, y, 76, 293, 88, 92))
    {
      saveShiftLightSettings();
      currentScreen = SCREEN_RPM;
      lastGaugeScreen = SCREEN_RPM;
      redrawUI();
      return;
    }

    // Bottom-center: PREV step
    if (isInRect(x, y, 189, 293, 88, 92))
    {
      selectedShiftStep--;

      if (selectedShiftStep < 0)
      {
        selectedShiftStep = SHIFT_STEP_COUNT - 1;
      }

      redrawUI();
      return;
    }

    // Bottom-right: BACK
    if (isInRect(x, y, 302, 293, 88, 92))
    {
      currentScreen = SCREEN_RPM;
      lastGaugeScreen = SCREEN_RPM;
      redrawUI();
      return;
    }
  }
}

void handleSwipe(int dx, int dy, unsigned long duration)
{
  if (duration > SWIPE_MAX_TIME)
  {
    return;
  }

  if (abs(dx) < SWIPE_MIN_DISTANCE && abs(dy) < SWIPE_MIN_DISTANCE)
  {
    return;
  }

  if (abs(dx) > abs(dy))
  {
    if (currentScreen == SCREEN_BATTERY ||
        currentScreen == SCREEN_ALL_IN_ONE ||
        currentScreen == SCREEN_RPM ||
        currentScreen == SCREEN_SPEED ||
        currentScreen == SCREEN_COOLANT ||
        currentScreen == SCREEN_THROTTLE ||
        currentScreen == SCREEN_INTAKE_AIR ||
        currentScreen == SCREEN_AIRFLOW ||
        currentScreen == SCREEN_MPG ||
        currentScreen == SCREEN_CHRONO ||
        currentScreen == SCREEN_TPMS ||
        currentScreen == SCREEN_OIL_TEMP)
    {
      if (dx < 0)
      {
        nextGauge();
      }
      else
      {
        previousGauge();
      }
    }

    return;
  }

  if (dy < 0)
  {
    nextRingColor();
  }
  else
  {
    previousRingColor();
  }
}

void handleTouch()
{
  int x;
  int y;

  bool hasPoint = readTouchPoint(x, y);
  unsigned long now = millis();

  if (hasPoint)
  {
    lastUserTouchActivityTime = now;
    lastTouchSeenTime = now;
    pendingTouchRelease = false;

    if (!touchActive)
    {
      touchActive = true;
      longPressTriggered = false;

      touchStartX = x;
      touchStartY = y;
      touchLastX = x;
      touchLastY = y;
      touchStartTime = now;

      longPressTarget = getLongPressTargetFromStart(touchStartX, touchStartY);

      repeatButtonActive = false;
      repeatButtonType = REPEAT_NONE;
      repeatButtonLastTime = now;

      if (currentScreen == SCREEN_SHIFT_SETTINGS)
      {
        if (isInRect(touchStartX, touchStartY, 76, 188, 88, 92))
        {
          repeatButtonType = REPEAT_MINUS;
          Serial.println("Repeat target = MINUS");
        }
        else if (isInRect(touchStartX, touchStartY, 302, 188, 88, 92))
        {
          repeatButtonType = REPEAT_PLUS;
          Serial.println("Repeat target = PLUS");
        }
      }

      Serial.print("Touch start x=");
      Serial.print(touchStartX);
      Serial.print(" y=");
      Serial.println(touchStartY);
    }
    else
    {
      touchLastX = x;
      touchLastY = y;
    }

    unsigned long heldTime = now - touchStartTime;

    int moveX = abs(touchLastX - touchStartX);
    int moveY = abs(touchLastY - touchStartY);

    bool fingerStayedClose = moveX <= LONG_PRESS_MOVE_LIMIT &&
                             moveY <= LONG_PRESS_MOVE_LIMIT;

    if (!longPressTriggered &&
        longPressTarget != LONG_TARGET_NONE &&
        fingerStayedClose &&
        heldTime >= LONG_PRESS_TIME)
    {
      longPressTriggered = true;
      triggerLongPressTarget();
    }

    return;
  }

  if (touchActive)
  {
    if (!pendingTouchRelease)
    {
      pendingTouchRelease = true;
      lastTouchSeenTime = now;
      return;
    }

    if (now - lastTouchSeenTime < TOUCH_RELEASE_GRACE)
    {
      return;
    }

    touchActive = false;
    pendingTouchRelease = false;
    lastUserTouchActivityTime = now;

    int dx = touchLastX - touchStartX;
    int dy = touchLastY - touchStartY;
    unsigned long duration = now - touchStartTime;

    Serial.print("Touch end x=");
    Serial.print(touchLastX);
    Serial.print(" y=");
    Serial.print(touchLastY);
    Serial.print(" duration=");
    Serial.println(duration);

    if (repeatButtonActive)
    {
      Serial.println("Release after repeat hold");

      repeatButtonActive = false;
      repeatButtonType = REPEAT_NONE;
      longPressTriggered = false;
      longPressTarget = LONG_TARGET_NONE;
      return;
    }

    if (longPressTriggered)
    {
      Serial.println("Release after long press");

      longPressTriggered = false;
      longPressTarget = LONG_TARGET_NONE;
      repeatButtonType = REPEAT_NONE;
      return;
    }

    int moveX = abs(touchLastX - touchStartX);
    int moveY = abs(touchLastY - touchStartY);

    bool fingerStayedClose = moveX <= LONG_PRESS_MOVE_LIMIT &&
                             moveY <= LONG_PRESS_MOVE_LIMIT;

    if (longPressTarget != LONG_TARGET_NONE &&
        fingerStayedClose &&
        duration >= LONG_PRESS_TIME)
    {
      Serial.println("Long press triggered on release");

      triggerLongPressTarget();

      longPressTriggered = false;
      longPressTarget = LONG_TARGET_NONE;
      repeatButtonType = REPEAT_NONE;
      return;
    }

    longPressTriggered = false;
    longPressTarget = LONG_TARGET_NONE;
    repeatButtonType = REPEAT_NONE;

    if (abs(dx) >= SWIPE_MIN_DISTANCE || abs(dy) >= SWIPE_MIN_DISTANCE)
    {
      handleSwipe(dx, dy, duration);
    }
    else
    {
      Serial.println("Tap detected");
      handleTap(touchLastX, touchLastY);
    }

    lastObdPollTime = millis();
  }
}

// =====================================================
// LVGL Root
// =====================================================

void createBlueLineUI()
{
  screenRoot = lv_screen_active();

  // Fully clear the boot/reconnect screen before building the gauge UI.
  // Without this, the boot labels/underline can remain behind transparent gauge layers.
  lv_obj_clean(screenRoot);
  gfx->fillScreen(RGB565_BLACK);

  ringLayer = nullptr;
  contentLayer = nullptr;

  lv_obj_set_style_bg_color(screenRoot, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(screenRoot, LV_OPA_COVER, LV_PART_MAIN);

  ringLayer = lv_obj_create(screenRoot);
  lv_obj_set_size(ringLayer, LCD_WIDTH, LCD_HEIGHT);
  lv_obj_center(ringLayer);
  lv_obj_set_style_bg_opa(ringLayer, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(ringLayer, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(ringLayer, 0, LV_PART_MAIN);
  lv_obj_clear_flag(ringLayer, LV_OBJ_FLAG_SCROLLABLE);

  contentLayer = lv_obj_create(screenRoot);
  lv_obj_set_size(contentLayer, LCD_WIDTH, LCD_HEIGHT);
  lv_obj_center(contentLayer);
  lv_obj_set_style_bg_opa(contentLayer, LV_OPA_TRANSP, LV_PART_MAIN);
  lv_obj_set_style_border_width(contentLayer, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(contentLayer, 0, LV_PART_MAIN);
  lv_obj_clear_flag(contentLayer, LV_OBJ_FLAG_SCROLLABLE);

  currentScreen = SCREEN_ALL_IN_ONE;
  lastGaugeScreen = SCREEN_ALL_IN_ONE;

  redrawUI();
}


// =====================================================
// Boot Animation
// =====================================================

void drawBootAnimation()
{
  // LVGL boot screen using the Xolonium 48pt font.
  // Clean straight underline with one white glide from left to right.
  lv_obj_t *bootScreen = lv_screen_active();
  lv_obj_clean(bootScreen);

  lv_obj_set_style_bg_color(bootScreen, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(bootScreen, LV_OPA_COVER, LV_PART_MAIN);

  const int lineW = 310;
  const int lineH = 4;
  const int lineX = (LCD_WIDTH - lineW) / 2;
  const int lineY = 280;
  const int shineW = 42;

  // Soft blue shadow/glow behind the title.
  lv_obj_t *glowLabel = lv_label_create(bootScreen);
  lv_label_set_text(glowLabel, "BlueLine One");
  lv_obj_set_width(glowLabel, LCD_WIDTH);
  lv_obj_set_style_text_font(glowLabel, &Xolonium_pn4D48pt7b, LV_PART_MAIN);
  lv_obj_set_style_text_color(glowLabel, lv_color_hex(0x0095FF), LV_PART_MAIN);
  lv_obj_set_style_text_opa(glowLabel, LV_OPA_50, LV_PART_MAIN);
  lv_obj_set_style_text_align(glowLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(glowLabel, LV_ALIGN_CENTER, 3, -35);

  // White title foreground.
  lv_obj_t *titleLabel = lv_label_create(bootScreen);
  lv_label_set_text(titleLabel, "BlueLine One");
  lv_obj_set_width(titleLabel, LCD_WIDTH);
  lv_obj_set_style_text_font(titleLabel, &Xolonium_pn4D48pt7b, LV_PART_MAIN);
  lv_obj_set_style_text_color(titleLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_set_style_text_align(titleLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(titleLabel, LV_ALIGN_CENTER, 0, -38);

  // Straight neon underline. Radius is 0 on purpose so it does not look bowed/curved.
  lv_obj_t *lineGlowOuter = lv_obj_create(bootScreen);
  lv_obj_set_size(lineGlowOuter, lineW + 24, 10);
  lv_obj_set_pos(lineGlowOuter, lineX - 12, lineY - 3);
  lv_obj_set_style_bg_color(lineGlowOuter, lv_color_hex(0x003A44), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(lineGlowOuter, LV_OPA_60, LV_PART_MAIN);
  lv_obj_set_style_border_width(lineGlowOuter, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(lineGlowOuter, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(lineGlowOuter, 0, LV_PART_MAIN);
  lv_obj_clear_flag(lineGlowOuter, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *lineGlowInner = lv_obj_create(bootScreen);
  lv_obj_set_size(lineGlowInner, lineW + 10, 6);
  lv_obj_set_pos(lineGlowInner, lineX - 5, lineY - 1);
  lv_obj_set_style_bg_color(lineGlowInner, lv_color_hex(0x007C90), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(lineGlowInner, LV_OPA_70, LV_PART_MAIN);
  lv_obj_set_style_border_width(lineGlowInner, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(lineGlowInner, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(lineGlowInner, 0, LV_PART_MAIN);
  lv_obj_clear_flag(lineGlowInner, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *blueLine = lv_obj_create(bootScreen);
  lv_obj_set_size(blueLine, lineW, lineH);
  lv_obj_set_pos(blueLine, lineX, lineY);
  lv_obj_set_style_bg_color(blueLine, lv_color_hex(0x00BFFF), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(blueLine, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(blueLine, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(blueLine, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(blueLine, 0, LV_PART_MAIN);
  lv_obj_clear_flag(blueLine, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *hotLine = lv_obj_create(bootScreen);
  lv_obj_set_size(hotLine, lineW, 1);
  lv_obj_set_pos(hotLine, lineX, lineY + 1);
  lv_obj_set_style_bg_color(hotLine, lv_color_hex(0xAAFFFF), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(hotLine, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(hotLine, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(hotLine, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(hotLine, 0, LV_PART_MAIN);
  lv_obj_clear_flag(hotLine, LV_OBJ_FLAG_SCROLLABLE);

  // White flash: straight rectangle riding directly on top of the underline.
  lv_obj_t *shine = lv_obj_create(bootScreen);
  lv_obj_set_size(shine, shineW, 8);
  lv_obj_set_pos(shine, lineX - shineW, lineY - 2);
  lv_obj_set_style_bg_color(shine, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(shine, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(shine, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(shine, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(shine, 0, LV_PART_MAIN);
  lv_obj_clear_flag(shine, LV_OBJ_FLAG_SCROLLABLE);

  // Let the boot logo appear before the single glide.
  for (int i = 0; i < 20; i++)
  {
    lv_tick_inc(5);
    lv_timer_handler();
    delay(5);
  }

  // One white flash glide from left to right across the blue line.
  for (int x = lineX - shineW; x <= lineX + lineW; x += 8)
  {
    lv_obj_set_pos(shine, x, lineY - 2);

    lv_tick_inc(5);
    lv_timer_handler();
    delay(12);
  }

  lv_obj_add_flag(shine, LV_OBJ_FLAG_HIDDEN);

  for (int i = 0; i < 45; i++)
  {
    lv_tick_inc(5);
    lv_timer_handler();
    delay(5);
  }

  lv_obj_clean(bootScreen);
}


void showBootReconnectStatus(const char *statusText, uint32_t statusColor)
{
  lv_obj_t *bootScreen = lv_screen_active();
  lv_obj_clean(bootScreen);

  lv_obj_set_style_bg_color(bootScreen, lv_color_hex(0x000000), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(bootScreen, LV_OPA_COVER, LV_PART_MAIN);

  const int lineW = 310;
  const int lineH = 4;
  const int lineX = (LCD_WIDTH - lineW) / 2;
  const int lineY = 280;

  lv_obj_t *glowLabel = lv_label_create(bootScreen);
  lv_label_set_text(glowLabel, "BlueLine One");
  lv_obj_set_width(glowLabel, LCD_WIDTH);
  lv_obj_set_style_text_font(glowLabel, &Xolonium_pn4D48pt7b, LV_PART_MAIN);
  lv_obj_set_style_text_color(glowLabel, lv_color_hex(0x0095FF), LV_PART_MAIN);
  lv_obj_set_style_text_opa(glowLabel, LV_OPA_50, LV_PART_MAIN);
  lv_obj_set_style_text_align(glowLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(glowLabel, LV_ALIGN_CENTER, 3, -50);

  lv_obj_t *titleLabel = lv_label_create(bootScreen);
  lv_label_set_text(titleLabel, "BlueLine One");
  lv_obj_set_width(titleLabel, LCD_WIDTH);
  lv_obj_set_style_text_font(titleLabel, &Xolonium_pn4D48pt7b, LV_PART_MAIN);
  lv_obj_set_style_text_color(titleLabel, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
  lv_obj_set_style_text_align(titleLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(titleLabel, LV_ALIGN_CENTER, 0, -53);

  lv_obj_t *statusLabel = lv_label_create(bootScreen);
  lv_label_set_text(statusLabel, statusText);
  lv_obj_set_width(statusLabel, LCD_WIDTH);
  lv_obj_set_style_text_font(statusLabel, &Xolonium_pn4D24pt7b, LV_PART_MAIN);
  lv_obj_set_style_text_color(statusLabel, lv_color_hex(statusColor), LV_PART_MAIN);
  lv_obj_set_style_text_align(statusLabel, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
  lv_obj_align(statusLabel, LV_ALIGN_CENTER, 0, 22);

  lv_obj_t *lineGlowOuter = lv_obj_create(bootScreen);
  lv_obj_set_size(lineGlowOuter, lineW + 24, 10);
  lv_obj_set_pos(lineGlowOuter, lineX - 12, lineY - 3);
  lv_obj_set_style_bg_color(lineGlowOuter, lv_color_hex(0x003A44), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(lineGlowOuter, LV_OPA_60, LV_PART_MAIN);
  lv_obj_set_style_border_width(lineGlowOuter, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(lineGlowOuter, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(lineGlowOuter, 0, LV_PART_MAIN);
  lv_obj_clear_flag(lineGlowOuter, LV_OBJ_FLAG_SCROLLABLE);

  lv_obj_t *blueLine = lv_obj_create(bootScreen);
  lv_obj_set_size(blueLine, lineW, lineH);
  lv_obj_set_pos(blueLine, lineX, lineY);
  lv_obj_set_style_bg_color(blueLine, lv_color_hex(0x00BFFF), LV_PART_MAIN);
  lv_obj_set_style_bg_opa(blueLine, LV_OPA_COVER, LV_PART_MAIN);
  lv_obj_set_style_border_width(blueLine, 0, LV_PART_MAIN);
  lv_obj_set_style_radius(blueLine, 0, LV_PART_MAIN);
  lv_obj_set_style_pad_all(blueLine, 0, LV_PART_MAIN);
  lv_obj_clear_flag(blueLine, LV_OBJ_FLAG_SCROLLABLE);

  for (int i = 0; i < 8; i++)
  {
    lv_tick_inc(5);
    lv_timer_handler();
    delay(5);
  }
}

void runBootAutoReconnect()
{
  // Only show/use boot auto reconnect when a saved BLE adapter exists.
  // If there is no saved adapter, skip this screen entirely and go straight to the gauge.
  if (connectedBleAddress.length() == 0)
  {
    return;
  }

  // Auto reconnect happens during the loading screen so the user can see what is happening.
  // Background auto reconnect remains disabled after boot so it does not block swipes later.
  showBootReconnectStatus("AUTO RECONNECT", 0xFFAA00);
  delay(250);

  showBootReconnectStatus("CONNECTING", 0xFFAA00);
  bool success = connectToSavedBleQuiet();

  if (success)
  {
    showBootReconnectStatus("CONNECTED", 0x00FF80);
  }
  else
  {
    showBootReconnectStatus("NOT CONNECTED", 0xFF0000);
  }

  delay(650);
}


// =====================================================
// Setup
// =====================================================

void setup()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println();
  Serial.println("BlueLine One v68 Clear Boot Before Gauge");

  Wire.begin(IIC_SDA, IIC_SCL);
  Wire.setClock(400000);

  loadSettings();

  if (!gfx->begin())
  {
    Serial.println("gfx->begin failed");

    while (1)
    {
      delay(100);
    }
  }

  applyBrightness();

  gfx->fillScreen(RGB565_BLACK);

  lv_init();

  display = lv_display_create(LCD_WIDTH, LCD_HEIGHT);
  lv_display_set_color_format(display, LV_COLOR_FORMAT_RGB565);
  lv_display_set_flush_cb(display, my_disp_flush);

  lv_display_set_buffers(
    display,
    draw_buf,
    NULL,
    sizeof(draw_buf),
    LV_DISPLAY_RENDER_MODE_PARTIAL
  );

  drawBootAnimation();

  setupTouch();
  setupChronographButtons();

  runBootAutoReconnect();

  createBlueLineUI();

  bootCompleteTime = millis();
  autoReconnectAttempts = 0;
  lastAutoReconnectAttemptTime = 0;

  for (int i = 0; i < 20; i++)
  {
    lv_tick_inc(5);
    lv_timer_handler();
    delay(5);
  }

  Serial.println("Ready");
  Serial.println("Swipe left/right = gauges");
  Serial.println("Swipe up/down = ring colors");
  Serial.println("Hold BT symbol 3 sec = BLE menu");
  Serial.println("Hold gear symbol 3 sec = shift setup");
  Serial.println("Hold MPH/KPH 3 sec = switch units");
  Serial.println("Tap +/- = 25 RPM");
  Serial.println("Hold +/- = 250 RPM repeat");
  Serial.println("BLE menu: SCAN selects device, RECONNECT uses saved device");
  Serial.println("Live OBD: ATRV battery, 010C RPM, 010D speed");
  Serial.println("Boot reconnect: only if saved BLE device exists");
  Serial.println("Chronograph: PWR = start/stop, BOOT = reset, swipe up/down = ring colors");
}

// =====================================================
// Loop
// =====================================================

void loop()
{
  handleTouch();
  handleRepeatButtonHold();
  handleChronographButtons();
  updateChronograph();
  handleAutoReconnect();
  pollObdLiveData();
  pollTpmsDebug();

  lv_tick_inc(5);
  lv_timer_handler();

  delay(5);
}