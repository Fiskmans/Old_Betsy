#pragma once


const float globalMeterScaleFactor = 100.f;

float operator""_m(long double aMeterValue);

float operator""_cm(long double aCentiMeterValue);

float operator""_m(size_t aMeterValue);

float operator""_cm(size_t aCentiMeterValue);
