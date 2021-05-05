#pragma once


const float globalMeterScaleFactor = 100.f;

float operator""m(long double aMeterValue);

float operator""cm(long double aCentiMeterValue);

float operator""m(size_t aMeterValue);

float operator""cm(size_t aCentiMeterValue);
