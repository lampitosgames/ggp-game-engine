#include "stdafx.h"
#include "TestComponent.h"

using namespace ECS;

TestComponent::TestComponent( int aData )
{
    testData = aData;
}


TestComponent::~TestComponent()
{
}

void TestComponent::TestFunction()
{
    printf( "Hey this is comp %d a test function! our data is %d \n", id, testData );
}