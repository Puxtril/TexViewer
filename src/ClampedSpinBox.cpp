#include "ClampedSpinbox.h"

ClampedSpinbox::ClampedSpinbox(const std::vector<int>& values)
{
    for (const auto& x : values)
        m_possibleValues.push_back(x);
}

void
ClampedSpinbox::stepBy(int steps)
{
    int const index = std::max((qsizetype)0, (m_possibleValues.indexOf(value()) + steps) % m_possibleValues.size());
    setValue(m_possibleValues.at(index));
}