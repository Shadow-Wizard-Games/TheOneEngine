#include "Easing.h"

#include <vector>

class EasingManager
{
public:
    EasingManager() {}
    ~EasingManager() {}

    // Add a new easing instance to the manager and return its index
    int AddEasing(double totalTime, double delay = 0.0, bool loop = false)
    {
        easings.push_back(Easing(totalTime, delay, loop));
        return easings.size() - 1;
    }

    // Update all easing instances
    void Update(double dt)
    {
        for (auto& easing : easings)
            easing.TrackTime(dt, easing.IsLooped());
    }

    // Reset all easing instances
    void ResetAll()
    {
        for (auto& easing : easings)
            easing.Reset();
    }

    Easing& GetEasing(int index)
    {
        return easings.at(index);
    }

    size_t GetNumEasings() const
    {
        return easings.size();
    }

private:
    std::vector<Easing> easings;
};