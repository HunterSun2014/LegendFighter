#pragma once
#define ROTATION_GAIN 0.004f    // Sensitivity adjustment for the look controller
#define MOVEMENT_GAIN 0.1f      // Sensitivity adjustment for the move controller

ref class MoveLookController sealed
{
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


public:

	MoveLookController();

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

	void OnKeyDown(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::KeyEventArgs^ args
	);

	void OnKeyUp(
		_In_ Windows::UI::Core::CoreWindow^ sender,
		_In_ Windows::UI::Core::KeyEventArgs^ args
	);

	// Set up the Controls that this controller supports
	void Initialize(_In_ Windows::UI::Core::CoreWindow^ window);

	void Update(Windows::UI::Core::CoreWindow ^window);

internal:
	// Accessor to set position of controller
	void SetPosition(_In_ DirectX::XMFLOAT3 pos);

	// Accessor to set position of controller
	void SetOrientation(_In_ float pitch, _In_ float yaw);

	// Returns the position of the controller object
	DirectX::XMFLOAT3 get_Position();

	// Returns the point  which the controller is facing
	DirectX::XMFLOAT3 get_LookPoint();


};  // class MoveLookController


