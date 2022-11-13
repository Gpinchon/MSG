#pragma once

#include <glm/vec2.hpp>
#include <memory>
#include <set>

class Player;
class Level;

namespace Game {
void SetCurrentLevel(std::shared_ptr<Level> currentLevel);
std::shared_ptr<Level> CurrentLevel();
void Update(float step);
int PlayerNumber();
int AddPlayer();
void MovePlayer(int player, const glm::vec2& input);
void Init();
void Start();
};