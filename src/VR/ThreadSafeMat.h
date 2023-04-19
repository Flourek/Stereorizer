#include <mutex>
#include <opencv2/opencv.hpp>

class ThreadSafeMat
{
public:
    // Constructor initializes Mat data
//    ThreadSafeMat(int rows, int cols, int type) : data(rows, cols, type) {}

    // Get a copy of the internal Mat data
    cv::Mat get() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return data.clone();
    }

    // Set the internal Mat data
    void set(const cv::Mat& newData)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        data = newData.clone();
    }

private:
    cv::Mat data;              // The internal Mat data
    mutable std::mutex mutex_; // Mutex to synchronize access to the Mat data
};