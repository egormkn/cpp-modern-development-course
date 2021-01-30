#pragma once

#include <istream>
#include <memory>

#include "node.h"

std::shared_ptr<Node> ParseCondition(std::istream& is);

void TestParseCondition();
