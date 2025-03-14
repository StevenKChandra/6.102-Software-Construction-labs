#ifndef TURTLE_ACTION_H
#define TURTLE_ACTION_H

#include <string>
#include "line_segment.h"

enum class ActionType { 
    FORWARD,
    TURN,
    COLOR,
};

class Action {
public:
    /**
     * Constructs a new Action with the given type, display string, and line segment.
     *
     * @param type the type of action
     * @param displayString the display string
     */
    Action(ActionType type, const std::string& displayString);

    /**
     * Returns the type of this action.
     *
     * @return the type
     */
    ActionType type() const;

    /**
     * Returns the display string of this action.
     *
     * @return the display string
     */
    std::string toString() const;

private:
    ActionType type_;
    std::string displayString_;
};

#endif