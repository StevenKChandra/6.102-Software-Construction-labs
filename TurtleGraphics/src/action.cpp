#include "action.h"

Action::Action(ActionType type, const std::string& displayString):
    type_(type), displayString_(displayString) {}

ActionType Action::type() const { return ActionType(type_); }

std::string Action::toString() const { return displayString_; }