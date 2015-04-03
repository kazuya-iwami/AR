#pragma once
#include <list>
#include "DxLib.h"
#include "object.h"
#include "utility.h"

using namespace std;

//グローバル変数

std::list<std::shared_ptr<CObject>> drawlist;

auto enemy1 = make_shared<CEnemy>(); //スマートポインタに配列が実装されていないため
auto enemy2 = make_shared<CEnemy>();
auto enemy3 = make_shared<CEnemy>();

int score=0;
int cursur_x=500;
int cursur_y=350;

string encode(COMMAND_NAME, int, int, int);