class MinMax
{
public:
    float min;
    float max;

    MinMax(float min, float max)
    {
        this->min = min;
        this->max = max;
    }

    MinMax()
    {
        this->min = -1;
        this->max = -1;
    }
};