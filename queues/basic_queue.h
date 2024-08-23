template<typename T>
class basic_queue {
public:
    virtual void push(const T &value) = 0;
    virtual T pop() = 0;
};