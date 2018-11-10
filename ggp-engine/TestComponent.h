#pragma once

#include "BaseComponent.h"

namespace ECS
{
    /// <summary>
    /// A test component to make sure that we can use ECS
    /// </summary>
    class TestComponent :
        public BaseComponent<TestComponent>
    {
    public:

        TestComponent();

        ~TestComponent();

        /// <summary>
        /// A temporary function to make sure that we can access the data
        /// on this component
        /// </summary>
        void TestFunction();

    private:

        int testData = 45;

    };

}