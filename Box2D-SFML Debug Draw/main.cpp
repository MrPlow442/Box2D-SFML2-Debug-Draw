// Debug draw for Box2D 2.2.1 - 2.3.0 for SFML 2.0 - main.cpp
// Copyright (C) 2013  Matija Lovrekovic
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <Box2D/Box2D.h>
#include <vector>
#include <sstream>

#include "SFMLDebugDraw.h"

//Class copied from http://code.google.com/p/box2d/source/browse/trunk/Box2D/Testbed/Framework/Test.cpp
//Copyright (c) 2011 Erin Catto http://box2d.org
class QueryCallback : public b2QueryCallback
{
public:
	QueryCallback(const b2Vec2& point)
	{
		m_point = point;
		m_fixture = NULL;
	}

	bool ReportFixture(b2Fixture* fixture)
	{
		b2Body* body = fixture->GetBody();
		if (body->GetType() == b2_dynamicBody)
		{
			bool inside = fixture->TestPoint(m_point);
			if (inside)
			{
				m_fixture = fixture;

				// We are done, terminate the query.
				return false;
			}
		}

		// Continue the query.
		return true;
	}

	b2Vec2 m_point;
	b2Fixture* m_fixture;
};


//Converts SFML's vector to Box2D's vector and downscales it so it fits Box2D's MKS units
template<typename T > 
b2Vec2 sfVecToB2Vec(sf::Vector2<T> vector)
{
	return b2Vec2(vector.x / sfdd::SCALE, vector.y / sfdd::SCALE);
}

//Creates a box body and returns a reference to it
b2Body* createSquare(b2World &world, sf::RenderWindow &window)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = sfVecToB2Vec(sf::Mouse::getPosition(window));
	b2Body* body = world.CreateBody(&bodyDef);


	b2PolygonShape boxShape;
	boxShape.SetAsBox(0.5f, 0.5f, b2Vec2(0.f, 0.f), 0);
	body->CreateFixture(&boxShape, 1.f);
	
	return body;
}

//Creates a circle body and returns a reference to it
b2Body* createCircle(b2World &world, sf::RenderWindow &window)
{
	b2BodyDef bodyDef;
	bodyDef.type = b2_dynamicBody;
	bodyDef.position = sfVecToB2Vec(sf::Mouse::getPosition(window));
	b2Body* body = world.CreateBody(&bodyDef);


	b2CircleShape circleShape;
	circleShape.m_radius = 0.5f;
	circleShape.m_p.SetZero();
	body->CreateFixture(&circleShape, 1.f);

	return body;
}

int main()
{
	/* Create window and limit the framerate */
	sf::RenderWindow window(sf::VideoMode(1024,768, 32), "Box2D - SFML Debug Draw Test");
	window.setFramerateLimit(120);

	/* Initialize font */
	sf::Font mainFont;
	if(!mainFont.loadFromFile("res\\Franchise-Bold-hinted.ttf")) // Set path to your font
	{
		return 1;
	}

	/* Initialize debug text */
	sf::Text fpsCounter;
	fpsCounter.setFont(mainFont);
	fpsCounter.setColor(sf::Color::White);

	sf::Text helpText;
	helpText.setFont(mainFont);
	helpText.setColor(sf::Color::White);
	helpText.setCharacterSize(20);
	helpText.setPosition(static_cast<float>(window.getSize().x) - static_cast<float>(window.getSize().x/ 2.f), 0.f  );

	std::string helpTextString = std::string("Press 1 to spawn a box\n") +
		                         std::string("Press 2 to spawn a ball\n") +
								 std::string("Press backspace to remove the most recent body\n") +
							     std::string("Press F1 to enable/disable drawing of shapes\n") +
								 std::string("Press F2 to enable/disable drawing of AABB's\n") +
								 std::string("Press F3 to enable/disable drawing of center of gravity for objects\n") +
								 std::string("Press Tab to show/hide this text");

	helpText.setString(helpTextString);

	/* Create a stringstream for conversion purposes */
	std::stringstream sstream;

	/* Create physical world and set it's gravity */
	b2World world(b2Vec2(0.f, 10.f));
	world.SetAllowSleeping(true);

	/* Initialize SFML Debug Draw */
	SFMLDebugDraw debugDraw(window);

	world.SetDebugDraw(&debugDraw);

	/* Set initial flags for what to draw */
	debugDraw.SetFlags(b2Draw::e_shapeBit); //Only draw shapes

	/* constants for time step and physics accuracy */
	const float timeStep = 1.0f / 60.0f;
	const int velocityIterations = 6;
	const int positionIterations = 2;

	/* Create the bounding box */
	b2BodyDef boundingBoxDef;
	boundingBoxDef.type = b2_staticBody;
	float xPos = (window.getSize().x / 2.f) / sfdd::SCALE;
	float yPos = 0.5f;
	boundingBoxDef.position.Set(xPos, yPos);

	b2Body* boundingBoxBody = world.CreateBody(&boundingBoxDef);

	b2PolygonShape boxShape;
	boxShape.SetAsBox((window.getSize().x) / sfdd::SCALE, 0.5f, b2Vec2(0.f, 0.f), 0.f);
	boundingBoxBody->CreateFixture(&boxShape, 1.0); //Top

	yPos = (window.getSize().y) / sfdd::SCALE - 1.f;
	boxShape.SetAsBox((window.getSize().x) / sfdd::SCALE, 0.5f, b2Vec2(0.f, yPos), 0.f);
	boundingBoxBody->CreateFixture(&boxShape, 1.f); //Bottom

	xPos -= 0.5f;
	boxShape.SetAsBox(0.5f, (window.getSize().y) / sfdd::SCALE, b2Vec2(-xPos, 0.f), 0.f);
	boundingBoxBody->CreateFixture(&boxShape, 1.f);//Left

	boxShape.SetAsBox(0.5f, (window.getSize().y) / sfdd::SCALE, b2Vec2(xPos, 0.f), 0.f);
	boundingBoxBody->CreateFixture(&boxShape, 1.f);//Right

	/* Mouse Joint */
	b2MouseJoint* mouseJoint = nullptr;
	b2BodyDef bodyDef;
	b2Body* ground = world.CreateBody(&bodyDef); //This is not the body of the bounding box
												 //This body exists to serve as an anchor point for the mouse joint

	sf::Clock deltaClock; //Clock used to measure FPS

	bool helpTextEnabled = true;

	std::vector<b2Body*> bodies;
	while(window.isOpen())
	{
		sf::Time deltaTime = deltaClock.restart();
		sf::Event event;
		while(window.pollEvent(event))
		{
			if(event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
				window.close();
			else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F1)
			{
				if(debugDraw.GetFlags() & b2Draw::e_shapeBit) debugDraw.ClearFlags(b2Draw::e_shapeBit);
				else debugDraw.AppendFlags(b2Draw::e_shapeBit);
			}
			else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F2)
			{
				if(debugDraw.GetFlags() & b2Draw::e_aabbBit) debugDraw.ClearFlags(b2Draw::e_aabbBit);
				else debugDraw.AppendFlags(b2Draw::e_aabbBit);
			}
			else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F3)
			{
				if(debugDraw.GetFlags() & b2Draw::e_centerOfMassBit) debugDraw.ClearFlags(b2Draw::e_centerOfMassBit);
				else debugDraw.AppendFlags(b2Draw::e_centerOfMassBit);
			}
			else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Tab)
			{
				if(helpTextEnabled) helpTextEnabled = false;
				else helpTextEnabled = true;
			}
			else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num1)
			{
				bodies.push_back(createSquare(world, window));
			}
			else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Num2)
			{
				bodies.push_back(createCircle(world, window));
			}
			else if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::BackSpace)
			{
				if(!bodies.empty())
				{
					world.DestroyBody(bodies.back());
					bodies.pop_back();
				}
			}
			// Following three events are copied almost completely from http://code.google.com/p/box2d/source/browse/trunk/Box2D/Testbed/Framework/Test.cpp
			// Copyright (c) 2011 Erin Catto http://box2d.org
			else if(event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left && mouseJoint == nullptr)
			{
				b2Vec2 mousePos = sfVecToB2Vec(sf::Mouse::getPosition(window));

				// Make a small box.
				b2AABB aabb;
				b2Vec2 d;
				d.Set(0.001f, 0.001f);
				aabb.lowerBound = mousePos + d;
				aabb.upperBound = mousePos - d;

				// Query the world for overlapping shapes.
				QueryCallback callback(mousePos);
				world.QueryAABB(&callback, aabb);

				if (callback.m_fixture)
				{
					b2Body* body = callback.m_fixture->GetBody();
					b2MouseJointDef md;
					md.bodyA = ground; //If bounding box body is used instead, the dynamic bodes can be dragged over the bounding box and we don't want that
					md.bodyB = body;
					md.target = mousePos;
					md.maxForce = 1000.0f * body->GetMass();
					mouseJoint = (b2MouseJoint*)world.CreateJoint(&md);
					body->SetAwake(true);
				}
			}
			else if(event.type == sf::Event::MouseMoved && mouseJoint != nullptr)
			{
				b2Vec2 mousePos = sfVecToB2Vec(sf::Mouse::getPosition(window));
				mouseJoint->SetTarget(mousePos);
			}
			else if(event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left && mouseJoint != nullptr)
			{
				world.DestroyJoint(mouseJoint);
				mouseJoint = nullptr;
			}
		}
		window.clear();
		world.Step(timeStep,velocityIterations,positionIterations);

		{//Display FPS
			sstream.precision(0);
			sstream << std::fixed << "FPS: " << 1.f / deltaTime.asSeconds();
			fpsCounter.setString(sstream.str());
			window.draw(fpsCounter);
			sstream.str("");
		}

		if (helpTextEnabled)
		{
			window.draw(helpText);
		}

		world.DrawDebugData();

		window.display();
	}

	return 0;
}