#include <nan.h>
#include "windows.h"
#include "xinput.h"

#pragma comment(lib, "XInput.lib")

using namespace std;

typedef int (*PowerOffController)(int);
typedef byte(*XINPUT_BATTERY_INFORMATION)(byte);
typedef byte(*BATTERY_DEVTYPE_GAMEPAD)(byte);

NAN_METHOD(off) {
  int index = Nan::To<int>(info[0]).FromJust()-1;

	if(index < 0 || index > XUSER_MAX_COUNT) {
	  return Nan::ThrowTypeError("invalid controller index");
	}

  HINSTANCE hXInputDLL = LoadLibraryA("XInput1_3.dll");

	if (hXInputDLL == NULL) {
	  return Nan::ThrowError("failed to load xinput dll");
	}

  XINPUT_STATE state;
  memset(&state, 0, sizeof(XINPUT_STATE));

  if(XInputGetState(index, &state) == ERROR_SUCCESS)
  {
    PowerOffController(*GetProcAddress(hXInputDLL, (LPCSTR) 103))(index);
  }

  ZeroMemory(&state, sizeof(XINPUT_STATE));

	FreeLibrary(hXInputDLL);
}

NAN_METHOD(offAll) {
  HINSTANCE hXInputDLL = LoadLibraryA("XInput1_3.dll");

	if (hXInputDLL == NULL) {
	  return Nan::ThrowError("failed to load xinput dll");
	}

	for(int i = 0; i < XUSER_MAX_COUNT; i++)
	{
		XINPUT_STATE state;
		memset(&state, 0, sizeof(XINPUT_STATE));

		if(XInputGetState(i, &state) == ERROR_SUCCESS)
		{
			PowerOffController(*GetProcAddress(hXInputDLL, (LPCSTR) 103))(i);
		}

		ZeroMemory(&state, sizeof(XINPUT_STATE));
	}

	FreeLibrary(hXInputDLL);
}

NAN_METHOD(list) {
  v8::Local<v8::Array> returnValue = Nan::New<v8::Array>();

  int arrayIndex = 0;

	for(int i = 0; i < XUSER_MAX_COUNT; i++)
	{
		XINPUT_STATE state;
		memset(&state, 0, sizeof(XINPUT_STATE));

		if(XInputGetState(i, &state) == ERROR_SUCCESS)
		{
		  returnValue->Set(arrayIndex++, Nan::New<v8::Number>(i+1));
		}

		ZeroMemory(&state, sizeof(XINPUT_STATE));
	}

  info.GetReturnValue().Set(returnValue);
}

NAN_METHOD(vibrate) {
  int index = Nan::To<int>(info[0]).FromJust()-1;

	if(index < 0 || index > XUSER_MAX_COUNT) {
	  return Nan::ThrowTypeError("invalid controller index");
	}

  XINPUT_VIBRATION Vibration;

  ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

  Vibration.wLeftMotorSpeed = 65535;
  Vibration.wRightMotorSpeed = 65535;

  XInputSetState(index, &Vibration);

  Sleep(1000);

  Vibration.wLeftMotorSpeed = 0;
  Vibration.wRightMotorSpeed = 0;

  XInputSetState(index, &Vibration);
}

NAN_METHOD(vibrateAll) {
  XINPUT_VIBRATION Vibration;

  ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));

  Vibration.wLeftMotorSpeed = 65535;
  Vibration.wRightMotorSpeed = 65535;

  for(int i = 0; i < XUSER_MAX_COUNT; i++)
  {
    XInputSetState(i, &Vibration);
  }

  Sleep(1000);

  Vibration.wLeftMotorSpeed = 0;
  Vibration.wRightMotorSpeed = 0;

  for(int i = 0; i < XUSER_MAX_COUNT; i++)
  {
    XInputSetState(i, &Vibration);
  }
}

NAN_METHOD(battery) {
  int index = Nan::To<int>(info[0]).FromJust()-1;

	if(index < 0 || index > XUSER_MAX_COUNT) {
	  return Nan::ThrowTypeError("invalid controller index");
	}

  XINPUT_BATTERY_INFORMATION BatteryInfo;
	memset(&BatteryInfo, 0, sizeof(XINPUT_STATE));

  if(XInputGetBatteryInformation(index, 0 /*0x00*/, &BatteryInfo) == ERROR_SUCCESS)
  {
    info.GetReturnValue().Set(BatteryInfo);
  }
}

NAN_METHOD(batteryInfoAll) {
  v8::Local<v8::Array> returnValue = Nan::New<v8::Array>();

  for (int i = 0; i < XUSER_MAX_COUNT; i++)
  {
    XINPUT_BATTERY_INFORMATION BatteryInfo;
    memset(&BatteryInfo, sizeof(XINPUT_BATTERY_INFORMATION));

    if (XInputGetBatteryInformation(index, BATTERY_DEVTYPE_GAMEPAD, &BatteryInfo) == ERROR_SUCCESS) {
      returnValue->Set(i, Nan::New<v8::Struct>(BatteryInfo));
    }
  }

  info.GetReturnValue().Set(returnValue);
}

NAN_MODULE_INIT(init) {
  NAN_EXPORT(target, off);
  NAN_EXPORT(target, offAll);
  NAN_EXPORT(target, list);
  NAN_EXPORT(target, vibrate);
  NAN_EXPORT(target, vibrateAll);
  NAN_EXPORT(target, battery);
  NAN_EXPORT(target, batteryAll);
}

NODE_MODULE(xinput, init)