/* Copyright (c) 2007 Scott Lembcke
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */
 
/*
	IMPORTANT - READ ME!
	
	This file sets up a simple interface that the individual demos can use to get
	a Chipmunk space running and draw what's in it. In order to keep the Chipmunk
	examples clean and simple, they contain no graphics code. All drawing is done
	by accessing the Chipmunk structures at a very low level. It is NOT
	recommended to write a game or application this way as it does not scale
	beyond simple shape drawing and is very dependent on implementation details
	about Chipmunk which may change with little to no warning.
*/

#include "chipmunkdemo.h"
#include "main.h"
#include "nv3030b.h"

#define WIDTH LCD_W
#define HEIGHT LCD_H
#define BALL_RADIUS 20

cpSpace *space;
cpBody *staticBody;
cpBody * ballBody;

void chipmunk_example_init(){
  // 创建物理空间
    space = cpSpaceNew();
    cpSpaceSetGravity(space, cpv(0, 100));

    // 创建静态物体，即边界
    staticBody = cpSpaceGetStaticBody(space);
    cpShape *shape = cpSegmentShapeNew(staticBody, cpv(0,0), cpv(0, HEIGHT), 0.0f);
    cpShapeSetFriction(shape, 1.0);
    cpShapeSetElasticity(shape, 1.0);
    cpSpaceAddShape(space, shape);
    shape = cpSegmentShapeNew(staticBody, cpv(0, HEIGHT), cpv(WIDTH, HEIGHT), 0.0f);
    cpShapeSetFriction(shape, 1.0);
    cpShapeSetElasticity(shape, 1.0);
    cpSpaceAddShape(space, shape);
    shape = cpSegmentShapeNew(staticBody, cpv(WIDTH, HEIGHT), cpv(WIDTH, 0), 0.0f);
    cpShapeSetFriction(shape, 1.0);
    cpShapeSetElasticity(shape, 1.0);
    cpSpaceAddShape(space, shape);
    shape = cpSegmentShapeNew(staticBody, cpv(WIDTH, 0), cpv(0, 0), 0.0f);
    cpShapeSetFriction(shape, 1.0);
    cpShapeSetElasticity(shape, 1.0);
    cpSpaceAddShape(space, shape);

    // // 创建1个小球
    cpFloat radius = 20.0;
    cpFloat x = cpflerp(radius * 2, WIDTH - radius * 2, 5);
    cpFloat y = cpflerp(radius * 2, HEIGHT - radius * 2, 5);

    ballBody = cpSpaceAddBody(space, cpBodyNew(1.0f, cpMomentForCircle(1.0f, 0.0f, radius, cpvzero)));
    cpBodySetPosition(ballBody, cpv(x, y));
    cpShape *ball_shape = cpSpaceAddShape(space, cpCircleShapeNew(ballBody, radius, cpvzero));
    cpShapeSetFriction(ball_shape, 0.7);
    cpShapeSetElasticity(ball_shape, 0.8);
    cpVect velocity = cpv(0.5, 0.8);
    cpBodySetVelocity(ballBody, velocity);
}

void chipmunk_example_update(float timems){
	cpSpaceStep(space, timems);

	printf("Ball Position: (%.2f, %.2f)\n", cpBodyGetPosition(ballBody).x, cpBodyGetPosition(ballBody).y);
//	NV3030B_ClearBuffer();
//	NV3030B_DrawCircle(cpBodyGetPosition(ballBody).x-20, cpBodyGetPosition(ballBody).y-20,BALL_RADIUS*2,RGB565_PINK, CIRCLE_DRAW_ALL);
//	NV3030B_SendBuffer();
}


