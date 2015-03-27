#pragma once
#include <list>
#include "DxLib.h"
#include "object.h"
#include "utility.h"

using namespace std;

std::list<std::shared_ptr<CObject>> drawlist;

auto enemy1 = make_shared<CEnemy>(); //スマートポインタに配列が実装されていないため
auto enemy2 = make_shared<CEnemy>();
auto enemy3 = make_shared<CEnemy>();

