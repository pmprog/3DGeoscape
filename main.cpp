
#include <irrlicht.h>
#include "driverChoice.h"
#include "polar.h"

#define IDFlag_IsNotPickable			0
#define IDFlag_IsPickable					1

using namespace irr;

IrrlichtDevice* device;

core::vector3df* GetLongitudeLatitudeOffset( core::vector3df* centre, core::vector2df* longlat, float radius );
core::vector2df* GetLongitudeLatitude( core::vector3df* centre, core::vector3df* cartesian, float radius );

/*
To receive events like mouse and keyboard input, or GUI events like "the OK
button has been clicked", we need an object which is derived from the
irr::IEventReceiver object. There is only one method to override:
irr::IEventReceiver::OnEvent(). This method will be called by the engine once
when an event happens. What we really want to know is whether a key is being
held down, and so we will remember the current state of each key.
*/
class MyEventReceiver : public IEventReceiver
{
	public:
		// This is the one method that we have to implement
		virtual bool OnEvent(const SEvent& event)
		{
				// Remember whether each key is down or up
				if (event.EventType == irr::EET_KEY_INPUT_EVENT)
						KeyIsDown[event.KeyInput.Key] = event.KeyInput.PressedDown;

				if( event.EventType == irr::EET_MOUSE_INPUT_EVENT)
				{
						MousePosition.X = event.MouseInput.X;
						MousePosition.Y = event.MouseInput.Y;
				}
				return false;
		}

		// This is used to check whether a key is being held down
		virtual bool IsKeyDown(EKEY_CODE keyCode) const
		{
				return KeyIsDown[keyCode];
		}

		virtual core::position2di GetMousePosition()
		{
				return MousePosition;
		}
		
		MyEventReceiver()
		{
				for (u32 i=0; i<KEY_KEY_CODES_COUNT; ++i)
						KeyIsDown[i] = false;
		}

private:
		core::position2di MousePosition;
		// We use this array to store the current state of each key
		bool KeyIsDown[KEY_KEY_CODES_COUNT];
};


/*
The event receiver for keeping the pressed keys is ready, the actual responses
will be made inside the render loop, right before drawing the scene. So lets
just create an irr::IrrlichtDevice and the scene node we want to move. We also
create some other additional scene nodes, to show that there are also some
different possibilities to move and animate scene nodes.
*/
int main()
{
		//core::vector2df* cameraLocation = new core::vector2df( 180, 180 );
		//f32 cameraZoom = 12.0;
		Polar* cameraLocation = new Polar( 0.0, 0.0, 12.0 );
		f32 cameraUpVctDst = (cameraLocation->Radius * cameraLocation->Radius) * 2;

		// ask user for driver
		video::E_DRIVER_TYPE driverType = video::EDT_DIRECT3D9; // DIRECT3D9; // driverChoiceConsole();
		if (driverType==video::EDT_COUNT)
				return 1;

		// create device
		MyEventReceiver receiver;

		IrrlichtDevice* tmpDevice = createDevice( video::EDT_NULL );
		

		device = createDevice(driverType, core::dimension2d<u32>(800, 600), 32, false, false, false, &receiver);
		//device = createDevice(driverType, tmpDevice->getVideoModeList()->getDesktopResolution(), tmpDevice->getVideoModeList()->getDesktopDepth(), true, false, false, &receiver);

		if (device == 0)
				return 1; // could not create selected driver.

		video::IVideoDriver* driver = device->getVideoDriver();
		scene::ISceneManager* smgr = device->getSceneManager();

		video::ITexture* bkgTexture = driver->getTexture("background.png");

		smgr->setAmbientLight( irr::video::SColorf( 0.3, 0.3, 0.3 ) );


		scene::ISceneNode* indicatorNode = smgr->addSphereSceneNode(0.4, 32,0, IDFlag_IsNotPickable);
		if (indicatorNode)
		{
				//indicatorNode->setPosition(core::vector3df(0,0,10));
				indicatorNode->setMaterialTexture(0, driver->getTexture("earth_day.jpg"));
				indicatorNode->setMaterialFlag(video::EMF_LIGHTING, true);
		}

		scene::IAnimatedMesh* worldMesh = smgr->addSphereMesh( L"WorldMesh", 5.0, 64, 64 );
		scene::ISceneNode * node = smgr->addAnimatedMeshSceneNode( worldMesh, 0, IDFlag_IsPickable ); // smgr->addSphereSceneNode(5.0, 128,0, IDFlag_IsPickable);
		scene::ITriangleSelector* nodeTS;
		if (node)
		{
				//node->setPosition(core::vector3df(0,0,10));
				//node->setRotation(core::vector3df(0,0,7));
				node->setMaterialTexture(0, driver->getTexture("earth_day.jpg"));
				node->setMaterialFlag(video::EMF_LIGHTING, true);

								nodeTS = smgr->createTriangleSelector( worldMesh, node );
								node->setTriangleSelector( nodeTS );
		}

		scene::ISceneNode* ln = smgr->addLightSceneNode( 0, core::vector3df(0, 0, 0), video::SColorf(1.0, 1.0, 1.0), 500.0, IDFlag_IsNotPickable );
		scene::ISceneNodeAnimator* anim =
				smgr->createFlyCircleAnimator(core::vector3df(0,0,0), 150.0f);
		if (anim)
		{
				ln->addAnimator(anim);
				anim->drop();
		}


		/*
		To be able to look at and move around in this scene, we create a first
		person shooter style camera and make the mouse cursor invisible.
		*/
		scene::ICameraSceneNode* cam = smgr->addCameraSceneNode();
		cam->setID( IDFlag_IsNotPickable );
		device->getCursorControl()->setVisible(true);

		/*
		We have done everything, so lets draw it. We also write the current
		frames per second and the name of the driver to the caption of the
		window.
		*/
		int lastFPS = -1;

		// In order to do framerate independent movement, we have to know
		// how long it was since the last frame
		u32 then = device->getTimer()->getTime();

		// This is the movemen speed in units per second.
		const f32 MOVEMENT_SPEED = 60.0f;
		const f32 ZOOM_SPEED = 3.0f;

		while(device->run())
		{
				// Work out a frame delta time.
				const u32 now = device->getTimer()->getTime();
				const f32 frameDeltaTime = (f32)(now - then) / 1000.f; // Time in seconds
				then = now;

				/* Check if keys W, S, A or D are being held down, and move the
				sphere node around respectively. */
				core::vector3df nodePosition = node->getPosition();

				if(receiver.IsKeyDown(irr::KEY_KEY_W))
				{
						//nodePosition.Y += MOVEMENT_SPEED * frameDeltaTime;
						//r.rotateXYBy( 0.8, node->getPosition() );
						cameraLocation->Latitude += MOVEMENT_SPEED * frameDeltaTime;
				}
						
				else if(receiver.IsKeyDown(irr::KEY_KEY_S))
				{
						//nodePosition.Y -= MOVEMENT_SPEED * frameDeltaTime;
						//r.rotateXYBy( 0.8, node->getPosition() );
						cameraLocation->Latitude -= MOVEMENT_SPEED * frameDeltaTime;
				}

				if(receiver.IsKeyDown(irr::KEY_KEY_A))
				{
						//nodePosition.X -= MOVEMENT_SPEED * frameDeltaTime;
						//r.rotateXZBy( 0.8, node->getPosition() );
						cameraLocation->Longitude -= MOVEMENT_SPEED * frameDeltaTime;
				}
				else if(receiver.IsKeyDown(irr::KEY_KEY_D))
				{
						//nodePosition.X += MOVEMENT_SPEED * frameDeltaTime;
						//r.rotateXZBy( -0.8, node->getPosition() );
						cameraLocation->Longitude += MOVEMENT_SPEED * frameDeltaTime;
				}

				else if(receiver.IsKeyDown(irr::KEY_KEY_E))
				{
						cameraLocation->Radius += ZOOM_SPEED * frameDeltaTime;
						if( cameraLocation->Radius > 12.0 )
								cameraLocation->Radius = 12.0;
						cameraUpVctDst = (cameraLocation->Radius * cameraLocation->Radius) * 2;
				}
				else if(receiver.IsKeyDown(irr::KEY_KEY_Q))
				{
						cameraLocation->Radius -= ZOOM_SPEED * frameDeltaTime;
						if( cameraLocation->Radius < 6.0 )
								cameraLocation->Radius = 6.0;
						cameraUpVctDst = (cameraLocation->Radius * cameraLocation->Radius) * 2;
				}


				//core::vector3df* nodeLocalPos = new core::vector3df( node->getPosition().X, node->getPosition().Y, node->getPosition().Z );

				if( cameraLocation->Latitude < 0 )
						cameraLocation->Latitude += 360;
				if( cameraLocation->Longitude < 0 )
						cameraLocation->Longitude += 360;
				if( cameraLocation->Latitude >= 360 )
						cameraLocation->Latitude -= 360;
				if( cameraLocation->Longitude >= 360 )
						cameraLocation->Longitude -= 360;

				cam->setPosition( cameraLocation->ToCartesian() );  // *GetLongitudeLatitudeOffset( nodeLocalPos, cameraLocation, cameraZoom ) );
				cam->setTarget( node->getPosition() );
				

				driver->beginScene(true, true, video::SColor(255,113,113,133));

				driver->draw2DImage( bkgTexture, core::rect<s32>(0,0, driver->getScreenSize().Width, driver->getScreenSize().Height), core::rect<s32>(0,0, bkgTexture->getSize().Width, bkgTexture->getSize().Height) );
				smgr->drawAll(); // draw the 3d scene
				device->getGUIEnvironment()->drawAll(); // draw the gui environment (the logo)


				Polar* camUpRot = new Polar( cameraLocation->Longitude, cameraLocation->Latitude - 45, cameraUpVctDst );
				core::vector3df camUp = camUpRot->ToCartesian();
				camUp = cam->getPosition() - camUp;
				cam->setUpVector( camUp.normalize() );
				
				

				core::vector3df camInv = cam->getRotation();
				camInv = camInv.invert().normalize();
				core::vector3df mousePosition;
				core::triangle3df collisionTiangle;
				scene::ISceneNode* hitCheck = 0;

				scene::ISceneCollisionManager* cmgr = smgr->getSceneCollisionManager();
				core::line3df ray = cmgr->getRayFromScreenCoordinates(receiver.GetMousePosition(), cam);

				hitCheck = cmgr->getSceneNodeAndCollisionPointFromRay( ray, mousePosition, collisionTiangle, IDFlag_IsPickable, 0 );

				if( hitCheck == node )
				{
					indicatorNode->setPosition( mousePosition );
					
				}

				Polar* polarMouse = new Polar( cameraLocation->ToCartesian(), cameraLocation->Radius );
				printf( "Cam:Lon/Lat: %f, %f\t%f, %f\n", cameraLocation->Longitude, cameraLocation->Latitude , polarMouse->Longitude, polarMouse->Latitude );

				indicatorNode->setRotation( cam->getRotation() );

				driver->endScene();

				int fps = driver->getFPS();




				/*
				if (lastFPS != fps)
				{
						core::stringw tmp(L"Movement Example - Irrlicht Engine [");
						tmp += driver->getName();
						tmp += L"] fps: ";
						tmp += fps;

						device->setWindowCaption(tmp.c_str());
						lastFPS = fps;
				}
				*/
		}

		/*
		In the end, delete the Irrlicht device.
		*/
		device->drop();
		
		return 0;
}


core::vector3df* GetLongitudeLatitudeOffset( core::vector3df* centre, core::vector2df* longlat, float radius )
{
	float longitude = core::degToRad(longlat->X);
	float latitude = core::degToRad(longlat->Y);
	core::vector3df* converted = new core::vector3df(0, 0, 0);
	converted->X = centre->X + (-radius * cos(latitude) * cos(longitude));
	converted->Y = centre->Y + (radius * sin(latitude));
	converted->Z = centre->Z + (radius * cos(latitude) * sin(longitude));


	return converted;
}

core::vector2df* GetLongitudeLatitude( core::vector3df* centre, core::vector3df* cartesian, float radius )
{
	core::vector2df* converted = new core::vector2df(0, 0);
	//radius = ((cartesian->X - centre->X) * (cartesian->X - centre->X)) + ((cartesian->Y - centre->Y) * (cartesian->Y - centre->Y)) + ((cartesian->Z - centre->Z) * (cartesian->Z - centre->Z));

	// longitude
	converted->X = core::radToDeg(asin( (cartesian->Z - centre->Z) / radius )) + 180;


	converted->Y = core::radToDeg(atan2( (cartesian->Y - centre->Y), (cartesian->X - centre->X) ));
	if( converted->Y < 0 )
		converted->Y += 360;
	//converted->X = core::radToDeg(asin( cartesian->Z / radius ));
	//converted->Y = core::radToDeg(atan2( cartesian->Y, cartesian->X ));
	return converted;
}

