#pragma once

#include <QtWidgets/qspinbox.h>

class ClampedSpinbox : public QSpinBox
{
    QList<int> m_possibleValues;

public:
    ClampedSpinbox(const std::vector<int>& values);
    virtual void stepBy(int steps) override;
};