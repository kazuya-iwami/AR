#pragma once
#include <list>
#include "DxLib.h"
#include "object.h"
#include "utility.h"

using namespace std;

//ƒOƒ[ƒoƒ‹•Ï”

std::list<std::shared_ptr<CObject>> drawlist;

int score=0;
int cursur_x=500;
int cursur_y=350;

string encode(COMMAND_NAME, int, int, int);