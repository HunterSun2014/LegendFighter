#include "pch.h"
#include "MoveLookController.h"
#include "Log.h"

using namespace std;
using namespace Windows::UI::Core;
using namespace Windows::System;
using namespace Windows::Foundation;
using namespace Windows::Devices::Input;
#include <DirectXMath.h>
using namespace DirectX;
//
//	InputController
//
InputController::InputController(DirectX::XMFLOAT2 upperLeft, DirectX::XMFLOAT2 bottomRight) : 
	m_shapeType(ShapeType ::rectangle),
	m_pointerId(0),
	m_inUse(false)
{
	m_shape.rectangle.upperLeft = upperLeft;
	m_shape.rectangle.bottomRight = bottomRight;
}

InputController::InputController(DirectX::XMFLOAT2 center, float radius) :
	m_shapeType(ShapeType::circle),
	m_pointerId(0),
	m_inUse(false)
{
	m_shape.circle.center = center;
	m_shape.circle.radius = radius;
}

bool InputController::PointIn(DirectX::XMFLOAT2 point)
{
	if (m_shapeType == ShapeType::rectangle)
	{
		return	point.x > m_shape.rectangle.upperLeft.x &&
			point.x < m_shape.rectangle.bottomRight.x &&
			point.y >  m_shape.rectangle.upperLeft.y &&
			point.y < m_shape.rectangle.bottomRight.y;
	}
	else if (m_shapeType == ShapeType::circle)
	{
		XMVECTOR m = XMLoadFloat2(&point) - XMLoadFloat2(&m_shape.circle.center);
		XMVECTOR lengthSq = XMVector2LengthSq(m);

		return XMVectorGetX(lengthSq) < m_shape.circle.radius * m_shape.circle.radius;
	}

	return false;
}

void InputController::OnPointerPressed(uint32 pointerId, DirectX::XMFLOAT2 current)
{
	SetPointerId(pointerId);
	SetFirstDown(current);
	SetInUse(true);

	if(OnPressEvent)
		OnPressEvent(current);
}

void InputController::OnPointerMove(DirectX::XMFLOAT2 current)
{
	
}

void InputController::OnPointerReleased(DirectX::XMFLOAT2 current)
{
	SetInUse(false);
	SetPointerId(0);
	SetFirstDown(XMFLOAT2(0, 0));
	SetMovePos(XMFLOAT2(0, 0));
}
//
//	MoveController
//
MoveController::MoveController(DirectX::XMFLOAT2 upperLeft, DirectX::XMFLOAT2 bottomRight) : InputController(upperLeft, bottomRight)
{

}

MoveController::MoveController(DirectX::XMFLOAT2 center, float radius) : InputController(center, radius)
{

}

void MoveController::OnPointerPressed(uint32 pointerId, DirectX::XMFLOAT2 current)
{
	InputController::OnPointerPressed(pointerId, current);
}
void MoveController::OnPointerMove(DirectX::XMFLOAT2 current)
{
	SetMovePos(current);

	XMVECTOR sub = XMLoadFloat2(&m_movePos) - XMLoadFloat2(&m_firstDownPos);
	XMVECTOR length = XMVector2LengthSq(sub);

	if (XMVectorGetX(length) > 16 * 16)
	{
		sub = XMVector2Normalize(sub) * 16;
	}

	XMFLOAT2 move;
	XMStoreFloat2(&move, sub);

	if (OnMoveEvent)
		OnMoveEvent(move);
}
void MoveController::OnPointerReleased(DirectX::XMFLOAT2 current)
{
	InputController::OnPointerReleased(current);
}

//
//	LookController
//
void LookController::OnPointerPressed(uint32 pointerId, DirectX::XMFLOAT2 current)
{
	SetPointerId(pointerId);
	SetFirstDown(current);
	SetInUse(true);
}
void LookController::OnPointerMove(DirectX::XMFLOAT2 current)
{

}
void LookController::OnPointerReleased(DirectX::XMFLOAT2 current)
{

}
//
//	FireController
//
void FireController::OnPointerPressed(uint32 pointerId, DirectX::XMFLOAT2 current)
{
	SetPointerId(pointerId);
	SetFirstDown(current);
	SetInUse(true);
}

void FireController::OnPointerMove(DirectX::XMFLOAT2 current)
{

}

void FireController::OnPointerReleased(DirectX::XMFLOAT2 current)
{

}
//
//	MoveLookControl
//
MoveLookController::MoveLookController(CoreWindow ^window, Windows::Foundation::Size logicalSize)
{
	Initialize(window, logicalSize);
}
void MoveLookController::Initialize(_In_ CoreWindow^ window,
	Windows::Foundation::Size logicalSize)
{

	// Opt in to recieve touch/mouse events.
	window->PointerPressed +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MoveLookController::OnPointerPressed);

	window->PointerMoved +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MoveLookController::OnPointerMoved);

	window->PointerReleased +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MoveLookController::OnPointerReleased);

	window->PointerExited +=
		ref new TypedEventHandler<CoreWindow^, PointerEventArgs^>(this, &MoveLookController::OnPointerExited);

	window->KeyDown +=
		ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &MoveLookController::OnKeyDown);

	window->KeyUp +=
		ref new TypedEventHandler<CoreWindow^, KeyEventArgs^>(this, &MoveLookController::OnKeyUp);

	// Initialize the state of the controller.
	m_moveInUse = FALSE;                // No pointer is in the Move control.
	m_movePointerID = 0;

	m_lookInUse = FALSE;                // No pointer is in the Look control.
	m_lookPointerID = 0;

	//  Need to init this as it is reset every frame.
	m_moveCommand = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	SetOrientation(0, 0);             // Look straight ahead when the app starts.

	m_position.x = m_position.y = m_position.z = 0.f;

	//
	//	Create a move controller 
	//
	auto moveController = make_shared<MoveController>(XMFLOAT2(120, logicalSize.Height - 80), 60.f);
	m_inputControllers.push_back(moveController);

	auto fireController0 = make_shared<InputController>(XMFLOAT2(logicalSize.Width - 60, logicalSize.Height - 60), 50.0f );
	m_inputControllers.push_back(fireController0);

	auto fireController1 = make_shared<InputController>(XMFLOAT2(logicalSize.Width - 180, logicalSize.Height - 60), 50.0f);
	m_inputControllers.push_back(fireController1);
}

//
// class MoveLookController
//
void MoveLookController::OnPointerPressed(
	_In_ CoreWindow^ sender,
	_In_ PointerEventArgs^ args)
{
	// Get the current pointer position.
	uint32 pointerID = args->CurrentPoint->PointerId;
	DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y);

	
	auto device = args->CurrentPoint->PointerDevice;
	auto deviceType = device->PointerDeviceType;
	if (deviceType == PointerDeviceType::Mouse)
	{
		// Action, Jump, or Fire
	}

#ifdef _DEBUG
	Log().Get() << L"OnPointerPressed :"
		<< L"id = " << pointerID << L","
		<< L"pos.x = " << position.x << L","
		<< L"pos.y = " << position.y
		<< endl;
#endif

	auto controller = find_if(begin(m_inputControllers), end(m_inputControllers), [&](auto iter)
	{
		//Check if this pointer is in the rectangle of input controller
		return iter-> PointIn(position);
	});

	if (controller != end(m_inputControllers))
	{
		(*controller)->OnPointerPressed(pointerID, position);
	}

}


void MoveLookController::OnPointerMoved(
	_In_ CoreWindow ^sender,
	_In_ PointerEventArgs ^args)
{
	uint32 pointerID = args->CurrentPoint->PointerId;
	DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y);

#ifdef _DEBUG1
	Log().Get() << L"Moved" 
		<< L"id = " << pointerID << L","
		<< L"pos.x = " << position.x << L","
		<< L"pos.y = " << position.y
		<< endl;
#endif

	auto controller = find_if(begin(m_inputControllers), end(m_inputControllers), [&](auto iter)
	{
		return pointerID == iter->PointId();
	});

	if (controller != end(m_inputControllers))
	{
		(*controller)->OnPointerMove(position);
	}

	// Decide which control this pointer is operating.
	if (pointerID == m_movePointerID)           // This is the move pointer.
	{
		// Move control
		m_movePointerPosition = position;       // Save the current position.		
	}
	else if (pointerID == m_lookPointerID)      // This is the look pointer.
	{
		// Look control

		DirectX::XMFLOAT2 pointerDelta;
		pointerDelta.x = position.x - m_lookLastPoint.x;        // How far did pointer move
		pointerDelta.y = position.y - m_lookLastPoint.y;

		DirectX::XMFLOAT2 rotationDelta;
		rotationDelta.x = pointerDelta.x * ROTATION_GAIN;   // Scale for control sensitivity.
		rotationDelta.y = pointerDelta.y * ROTATION_GAIN;

		m_lookLastPoint = position;                     // Save for the next time through.

														// Update our orientation based on the command.
		m_pitch -= rotationDelta.y;                     // Mouse y increases down, but pitch increases up.
		m_yaw -= rotationDelta.x;                       // Yaw is defined as CCW around the y-axis.

														// Limit the pitch to straight up or straight down.
		m_pitch = (float)__max(-DirectX::XM_PI / 2.0f, m_pitch);
		m_pitch = (float)__min(+DirectX::XM_PI / 2.0f, m_pitch);
	}
}

void MoveLookController::OnPointerReleased(
	_In_ CoreWindow ^sender,
	_In_ PointerEventArgs ^args)
{
	uint32 pointerID = args->CurrentPoint->PointerId;
	DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y);
	
#ifdef _DEBUG
	Log().Get() << L"Release"
		<< L"id = " << pointerID << L","
		<< L"pos.x = " << position.x << L","
		<< L"pos.y = " << position.y
		<< endl;
#endif

	auto controller = find_if(begin(m_inputControllers), end(m_inputControllers), [=](auto iter)
	{
		return pointerID == iter->PointId();
	});

	if (controller != end(m_inputControllers))
	{
		(*controller)->OnPointerReleased(position);

		if ((*controller)->OnReleaseEvent)
			(*controller)->OnReleaseEvent(position);
	}

	if (pointerID == m_movePointerID)    // This was the move pointer.
	{
		m_moveInUse = FALSE;
		m_movePointerID = 0;
		m_position.x = m_position.y = m_position.z = 0;
	}
	else if (pointerID == m_lookPointerID) // This was the look pointer.
	{
		m_lookInUse = FALSE;
		m_lookPointerID = 0;
	}
}

void MoveLookController::OnPointerExited(
	_In_ Windows::UI::Core::CoreWindow^ sender,
	_In_ Windows::UI::Core::PointerEventArgs^ args
)
{

}

void MoveLookController::OnKeyDown(
	__in CoreWindow^ sender,
	__in KeyEventArgs^ args)
{
	Windows::System::VirtualKey Key;
	Key = args->VirtualKey;

	// Figure out the command from the keyboard.
	if (Key == VirtualKey::W)     // Forward
		m_forward = true;
	if (Key == VirtualKey::S)     // Back
		m_back = true;
	if (Key == VirtualKey::A)     // Left
		m_left = true;
	if (Key == VirtualKey::D)     // Right
		m_right = true;
}

void MoveLookController::OnKeyUp(
	__in CoreWindow^ sender,
	__in KeyEventArgs^ args)
{
	Windows::System::VirtualKey Key;
	Key = args->VirtualKey;

	// Figure out the command from the keyboard.
	if (Key == VirtualKey::W)     // forward
		m_forward = false;
	if (Key == VirtualKey::S)     // back
		m_back = false;
	if (Key == VirtualKey::A)     // left
		m_left = false;
	if (Key == VirtualKey::D)     // right
		m_right = false;
}


// Accessor to set the position of the controller.
void MoveLookController::SetOrientation(_In_ float pitch, _In_ float yaw)
{
	m_pitch = pitch;
	m_yaw = yaw;
}

// Returns the position of the controller object.
DirectX::XMFLOAT3 MoveLookController::Velocity()
{
	return m_position;
}

// Returns the point at which the camera controller is facing.
DirectX::XMFLOAT3 MoveLookController::get_LookPoint()
{
	float y = sinf(m_pitch);        // Vertical
	float r = cosf(m_pitch);        // In the plane
	float z = r*cosf(m_yaw);        // Fwd-back
	float x = r*sinf(m_yaw);        // Left-right
	DirectX::XMFLOAT3 result(x, y, z);
	result.x += m_position.x;
	result.y += m_position.y;
	result.z += m_position.z;

	// Return m_position + DirectX::XMFLOAT3(x, y, z);
	return result;
}

void MoveLookController::Update( )
{
	/*
	// Check for input from the Move control.
	if (m_moveInUse)
	{
		DirectX::XMFLOAT2 pointerDelta(m_movePointerPosition);
		pointerDelta.x -= m_moveFirstDown.x;
		pointerDelta.y -= m_moveFirstDown.y;

		// Figure out the command from the touch-based virtual joystick.
		if (pointerDelta.x > 16.0f)      // Leave 32 pixel-wide dead spot for being still.
			m_moveCommand.x = 1.0f;
		else
			if (pointerDelta.x < -16.0f)
				m_moveCommand.x = -1.0f;

		if (pointerDelta.y > 16.0f)      // Joystick y is up, so change sign.
			m_moveCommand.y = -1.0f;
		else
			if (pointerDelta.y < -16.0f)
				m_moveCommand.y = 1.0f;
	}

	// Poll our state bits that are set by the keyboard input events.
	if (m_forward)
		m_moveCommand.y += 1.0f;
	if (m_back)
		m_moveCommand.y -= 1.0f;

	if (m_left)
		m_moveCommand.x -= 1.0f;
	if (m_right)
		m_moveCommand.x += 1.0f;

	if (m_up)
		m_moveCommand.z += 1.0f;
	if (m_down)
		m_moveCommand.z -= 1.0f;

	// Make sure that 45 degree cases are not faster.
	DirectX::XMFLOAT3 command = m_moveCommand;
	DirectX::XMVECTOR vector;
	vector = DirectX::XMLoadFloat3(&command);

	if (fabsf(command.x) > 0.1f || fabsf(command.y) > 0.1f || fabsf(command.z) > 0.1f)
	{
		vector = DirectX::XMVector3Normalize(vector);
		DirectX::XMStoreFloat3(&command, vector);
	}

	// Rotate command to align with our direction (world coordinates).
	DirectX::XMFLOAT3 wCommand;
	wCommand.x = command.x*cosf(m_yaw) - command.y*sinf(m_yaw);
	wCommand.y = command.x*sinf(m_yaw) + command.y*cosf(m_yaw);
	wCommand.z = command.z;

	// Scale for sensitivity adjustment.
	wCommand.x = wCommand.x * MOVEMENT_GAIN;
	wCommand.y = wCommand.y * MOVEMENT_GAIN;
	wCommand.z = wCommand.z * MOVEMENT_GAIN;

	// Our velocity is based on the command.
	// Also note that y is the up-down axis. 
	DirectX::XMFLOAT3 Velocity;
	Velocity.x = wCommand.x;
	Velocity.z = wCommand.y;
	Velocity.y = wCommand.z;

	// Integrate
	m_position.x += Velocity.x;
	m_position.y += Velocity.y;
	m_position.z += Velocity.z;

	// Clear movement input accumulator for use during the next frame.
	m_moveCommand = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	*/
	//Velocity.x = 1, Velocity.z = -1;	//for test
	for (auto controller : m_inputControllers)
	{
		if (controller->OnUsingEvent)
		{
			if (controller->IsUsing())
				controller->OnUsingEvent(XMFLOAT2(0, 0));
		}		
	}
}

void MoveLookController::JsRegister(jsrt::context context, jsrt::object parent)
{
	//Resitry a Gamepad object
	m_jsGamepad = jsrt::object::create();
	parent.set_property(jsrt::property_id::create(L"Gamepad"), m_jsGamepad);

	auto moveController = m_inputControllers[0];
	moveController->OnMoveEvent = [=](XMFLOAT2 velocity)
	{		
		jsrt::context::scope scope(context);

		//velocity = XMLoadFloat2(&velocity) * m_deviceResources->GetOrientationTransform2D();

		auto onMove = m_jsGamepad.get_property<jsrt::function<void, double, double>>(jsrt::property_id::create(L"OnMove"));
		if (onMove != jsrt::context::undefined())
		{
			onMove(m_jsGamepad, velocity.x, -velocity.y);
			//Log().Get() << L"x = " << velocity.x << L" y = " << velocity.y << endl;
		}
	};

	moveController->OnReleaseEvent = [=](XMFLOAT2 position)
	{
		jsrt::context::scope scope(context);

		auto onMove = m_jsGamepad.get_property<jsrt::function<void, double, double>>(jsrt::property_id::create(L"OnMove"));
		if (onMove != jsrt::context::undefined())
		{
			//Set velocity to zero
			onMove(m_jsGamepad, 0,0);
			//Log().Get() << L"x = " << velocity.x << L" y = " << velocity.y << endl;
		}
	};

	auto fireController = m_inputControllers[1];
	fireController->OnPressEvent = [=](XMFLOAT2 current)
	{
		jsrt::context::scope scope(context);

		auto onFire = m_jsGamepad.get_property<jsrt::function<void,double>>(jsrt::property_id::create(L"OnFire"));
		if (onFire != jsrt::context::undefined())
		{
			onFire(m_jsGamepad, 0);
			//Log().Get() << L"x = " << velocity.x << L" y = " << velocity.y << endl;
		}				
	};

	fireController = m_inputControllers[2];
	fireController->OnPressEvent = [=](XMFLOAT2 current)
	{
		jsrt::context::scope scope(context);

		auto onFire = m_jsGamepad.get_property<jsrt::function<void, double>>(jsrt::property_id::create(L"OnFire"));
		if (onFire != jsrt::context::undefined())
		{
			onFire(m_jsGamepad, 1);
			//Log().Get() << L"x = " << velocity.x << L" y = " << velocity.y << endl;
		}
	};
}

//
//	Render
//
void MoveLookController::Render(const std::shared_ptr<DX::DeviceResources>& deviceResources)
{
	/*ID2D1DeviceContext* context = deviceResources->GetD2DDeviceContext();
	Windows::Foundation::Size logicalSize = deviceResources->GetLogicalSize();

	context->SaveDrawingState(m_stateBlock.Get());
	context->BeginDraw();

	{
		context->SetTransform(deviceResources->GetOrientationTransform2D());

		D2D1_ELLIPSE ellipse = D2D1::Ellipse(
			D2D1::Point2F(120.f, 320.f),
			60.f,
			60.f
		);

		context->DrawEllipse(ellipse, m_whiteBrush.Get());

		ellipse = D2D1::Ellipse(
			D2D1::Point2F(logicalSize.Width - 60, logicalSize.Height - 60),
			50.f,
			50.f
		);

		context->DrawEllipse(ellipse, m_whiteBrush.Get());
	}*/
}