#pragma once

class TestWindow {
public:
    TestWindow();

    static TestWindow* ShowWindow()
    {
        TestWindow* window = new TestWindow();
        return window;
    }
};