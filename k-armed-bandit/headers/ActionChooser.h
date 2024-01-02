//
// Created by silidrone on 30.12.2023.
//

#ifndef ACTIONCHOOSER_H
#define ACTIONCHOOSER_H

#include <vector>

class ActionChooser {
public:
    virtual int choose(std::vector<double>, std::vector<int>, int) = 0;
    virtual ~ActionChooser() = default;
};

#endif //ACTIONCHOOSER_H
