#pragma once
#include <pch.h>
#include "..\Common\DeviceResources.h"

#define ROTATION_GAIN 0.004f    // Sensitivity adjustment for the look controller
#define MOVEMENT_GAIN 1.0f      // Sensitivity adjustment for the move controller

class InputController
{
public:
	//Construct with rectangle
	InputController(DirectX::XMFLOAT2 upperLeft, DirectX::XMFLOAT2 bottomRight);

	//Construct with circle
	InputController(DirectX::XMFLOAT2 center, float radius);

	bool PointIn(DirectX::XMFLOAT2 center);

	void SetPointerId(uint32 pinterId) { m_pointerId = pinterId; }

	void SetFirstDown(DirectX::XMFLOAT2 pos) { m_firstDownPos = pos; }

	void SetMovePos(DirectX::XMFLOAT2 pos) { m_movePos = pos; }

	void SetInUse(bool used) { m_inUse = used; }

	uint32 PointId() { return m_pointerId; }

	bool IsUsing() { return m_inUse; }

	virtual void OnPointerPressed(uint32 pointerId, DirectX::XMFLOAT2 current);
	virtual void OnPointerMove(DirectX::XMFLOAT2 current);
	virtual void OnPointerReleased(DirectX::XMFLOAT2 current);
	
	std::function<void(DirectX::XMFLOAT2 current)> OnMoveEvent;	
	std::function<void(DirectX::XMFLOAT2 current)> OnPressEvent;
	std::function<void(DirectX::XMFLOAT2 current)> OnReleaseEvent;
	std::function<void(DirectX::XMFLOAT2 current)> OnUsingEvent;		//for auto firing

protected:	
	bool		m_inUse;
	uint32	m_pointerId;
	enum  ShapeType { rectangle, circle } m_shapeType;
	DirectX::XMFLOAT2 m_firstDownPos;
	DirectX::XMFLOAT2 m_movePos;

	union Shape
	{
		struct 
		{
			DirectX::XMFLOAT2	upperLeft;
			DirectX::XMFLOAT2	bottomRight;						
		} rectangle;

		struct 
		{
			DirectX::XMFLOAT2 center;
			float radius;
		} circle;

	} m_shape;	

private:
};

class MoveController : public InputController
{
public:
	MoveController(DirectX::XMFLOAT2 upperLeft, DirectX::XMFLOAT2 bottomRight);
	MoveController(DirectX::XMFLOAT2 center, float radius);

	virtual void OnPointerPressed(uint32 pointerId, DirectX::XMFLOAT2 current);
	virtual void OnPointerMove(DirectX::XMFLOAT2 current);
	virtual void OnPointerReleased(DirectX::XMFLOAT2 current);
};

class LookController : public InputController
{
public:
	virtual void OnPointerPressed(uint32 pointerId, DirectX::XMFLOAT2 current);
	virtual void OnPointerMove(DirectX::XMFLOAT2 current);
	virtual void OnPointerReleased(DirectX::XMFLOAT2 current);
};

class FireController : public InputController
{
public:
	virtual void OnPointerPressed(uint32 pointerId, DirectX::XMFLOAT2 current);
	virtual void OnPointerMove(DirectX::XMFLOAT2 current);
	virtual void OnPointerReleased(DirectX::XMFLOAT2 current);
};

ref class MoveLookController sealed
{
public:
	MoveLookController(Windows::UI::Core::CoreWindow ^window, Windows::Foundation::Size logicalSize);

	// Methods to get input from the UI pointers
	void OnPointerPressed(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::PointerEventArgs^ args
	);

	void OnPointerMoved(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::PointerEventArgs^ args
	);

	void OnPointerReleased(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::PointerEventArgs^ args
	);

	void OnPointerExited(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::PointerEventArgs^ args
	);

	void OnKeyDown(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::KeyEventArgs^ args
	);

	void OnKeyUp(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::KeyEventArgs^ args
	);

	// Set up the Controls that this controller supports
	void Initialize(_In_ Windows::UI::Core::CoreWindow^ window, Windows::Foundation::Size logicalSize);
		
internal:
	// Accessor to set position of controller
	void SetOrientation(_In_ float pitch, _In_ float yaw);

	// Returns the velocity of the controller object
	DirectX::XMFLOAT3 Velocity();

	// Returns the point  which the controller is facing
	DirectX::XMFLOAT3 get_LookPoint();

	void JsRegister(jsrt::context context, jsrt::object parent);

	void Update();

	void Render(const std::shared_ptr<DX::DeviceResources>& deviceResources);
private:
	// Properties of the controller object
	DirectX::XMFLOAT3 m_position;               // The position of the controller
	float m_pitch, m_yaw;           // Orientation euler angles in radians

									// Properties of the Move control
	bool m_moveInUse;               // Specifies whether the move control is in use
	uint32 m_movePointerID;         // Id of the pointer in this control
	DirectX::XMFLOAT2 m_moveFirstDown;          // Point where initial contact occurred
	DirectX::XMFLOAT2 m_movePointerPosition;   // Point where the move pointer is currently located
	DirectX::XMFLOAT3 m_moveCommand;            // The net command from the move control

												// Properties of the Look control
	bool m_lookInUse;               // Specifies whether the look control is in use
	uint32 m_lookPointerID;         // Id of the pointer in this control
	DirectX::XMFLOAT2 m_lookLastPoint;          // Last point (from last frame)
	DirectX::XMFLOAT2 m_lookLastDelta;          // For smoothing

	bool m_forward, m_back;         // States for movement
	bool m_left, m_right;
	bool m_up, m_down;

	std::vector<std::shared_ptr<InputController>> m_inputControllers;

	jsrt::object m_jsGamepad;

	std::shared_ptr<DX::DeviceResources> m_deviceResources;

};  // class MoveLookController
