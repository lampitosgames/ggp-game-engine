#include "stdafx.h"
#include "TestComponent.h"

using namespace ECS;

TestComponent::TestComponent()
{
}


TestComponent::~TestComponent()
{
}

void TestComponent::TestFunction()
{
    printf( "Hey this is comp %d a test function! our data is %d \n", id, testData );
}