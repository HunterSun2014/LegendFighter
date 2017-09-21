
//////////////////////////////////////////////////////
//  create the Go home state
/////////////////////////////////////////////////////
var stateGoHome = new Object();

stateGoHome.Enter = function (entity) {

}

stateGoHome.Execute = function (entity,  totalTime, elapsedTime) {

    //if miner is gatigued, then change the state to sleep
    //if (entity.Fatigued())
    //    entity.GetFSM().ChangeState(stateSleep);
    
    entity.SetRotation(0, Math.PI, totalTime * entity.speed);

}

stateGoHome.Exit = function (entity) {
    
}

//////////////////////////////////////////////////////
//  create the Sleep state
/////////////////////////////////////////////////////
var stateSleep = new Object();

stateSleep.Enter = function (miner) {

}

stateSleep.Execute = function (miner,  totalTime, elapsedTime) {
	try
	{
		var entity = miner;
		var fsm = miner.GetFSM();
		fsm.ChangeState(stateGoHome);
	}
	catch(err)
	{
	}
}

stateSleep.Exit = function (miner) {

}

//////////////////////////////////////////////////////
//  main function
/////////////////////////////////////////////////////
//debugger;
//
//	Create a fan and  set state machine
//
var fan = EntityManager.CreateEntity("assets\\Warrior\\fan.sdkmesh");
fan.name = "fan";

fan = EntityManager.FindEntity("assets\\Warrior\\fan.sdkmesh");
name = fan.name;
fan.speed = Math.PI / 8;

fan.SetScale(1.0, 1.0, 1.0);
fan.SetRotation(0.0, 0, 0.0);
fan.SetPosition(0.0, 7.24, 4.0);

fan.GetFSM().SetCurrentState(stateGoHome);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Create WindMill Stage
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
var windMillStage = EntityManager.CreateEntity("assets\\Warrior\\WindMillStage.sdkmesh");
windMillStage.name = "WindMillStae";

windMillStage.SetScale(2.0, 2.0, 2.0);
windMillStage.SetRotation(0.0, 0, 0.0);
windMillStage.SetPosition(0.0, 0, 0);
/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Create a solider 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
var soldier = EntityManager.CreateEntity("assets\\WalkingGirl\\WalkingGirl.sdkmesh");
soldier.name = "WalkingGirl";
var scale = 10;

soldier.SetScale(0.02, 0.02, 0.02 );
soldier.SetRotation(0.0, Math.PI  / 2, 0.0);
soldier.SetPosition(scale, 0.0, -scale);
//soldier.SetTranslation(0.0, 0.0, 0.0);
soldier.speed = 1.5;
soldier.speedTurn = Math.PI;
soldier.dir = Math.PI / 2;	//the initalization direction  of model
soldier.SetAnimation("WalkingGirl");

//  create the walk state
var stateWalk = new Object();

stateWalk.Enter = function (entity) {
	//debugger;
	var x = ( Math.random() * 256 ) ;
    var y = 0.0;
    var z = ( Math.random() * 256 ) ;
	
	
	this.path = [ [ scale, 0, -scale ],
						[scale, 0, scale ],
						[-scale, 0, scale ],
						[-scale, 0, -scale ],
						[scale, 0, -scale ]
					];
	
	this.pathIndex = 0;

	entity.SetTargetPos(scale, 0, -scale);
}

stateWalk.Execute = function (entity,  totalTime, elapsedTime) 
{	 
	var target = this.path[this.pathIndex];

	if(entity.pos.x == target[0] && entity.pos.y ==  target[1] && entity.pos.z ==  target[2])
	{
		this.pathIndex++;
		if(this.pathIndex >= this.path.length)
			this.pathIndex = 0
		
		target = this.path[this.pathIndex];

		entity.SetTargetPos(target[0], target[1], target[2]);
	}

	entity.Move(elapsedTime);	
	//entity.GetFSM().ChangeState(stateGoHome);
}

stateWalk.Exit = function (miner) {

}

soldier.GetFSM().SetCurrentState(stateWalk);

/*
var soldier2 = EntityManager.CreateEntity("assets\\Soldier\\soldier.sdkmesh");
soldier2.name = "soldier2";

soldier2.SetScale(4.0, 4.0, 4.0 );
soldier2.SetRotation(0.0, Math.PI  , 0.0);
soldier2.SetPosition(1.5, 0.0, -3.0);
soldier2.speed = 1.0;
soldier2.speedTurn = Math.PI;
soldier2.dir = -Math.PI /2;	//the initalization direction  of model
soldier2.ChooseNewLocation = function()
{
    var x = ( Math.Rand() * 256 );
    var y = 0.0;
    var z = ( Math.Rand() * 256 );
}

//  create the walk state for soldier 2
var stateWalk2 = new Object();

stateWalk2.Enter = function (entity) {

	var x = ( Math.random() * 256 ) ;
    var y = 0.0;
    var z = ( Math.random() * 256 ) ;

	entity.SetTargetPos(5, 0, 0);
}

stateWalk2.Execute = function (entity,  totalTime, elapsedTime) 
{	 

	if(entity.pos.x == 5 && entity.pos.z == 0)
		entity.SetTargetPos(0, 0, 5);
	else if(entity.pos.x == 0  && entity.pos.z == 5)
		entity.SetTargetPos(0, 0, -5);
	else if(entity.pos.x == 0 && entity.pos.z == -5)
		entity.SetTargetPos(-5, 0, 0);
	else if(entity.pos.x == -5 && entity.pos.z == 0)
		entity.SetTargetPos(5, 0, 0);

	entity.Move(elapsedTime);	
	//entity.GetFSM().ChangeState(stateGoHome);
}

stateWalk2.Exit = function (miner) {

}

soldier2.GetFSM().SetCurrentState(stateWalk2);

/*
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	create first cmo model
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
var soldier = EntityManager.CreateEntity("ThirdPersonWalk.cmo");

soldier.SetScale(0.02, 0.02, -0.02 );
soldier.SetRotation(Math.PI/2, 0, 0);

soldier.SetPosition(3.0, 0, 0);

soldier.SetAnimation("Unreal Take");

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	create second cmo model
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

var soldier = EntityManager.CreateEntity("ThirdPersonRun.cmo");

soldier.SetScale(0.02, 0.02, -0.02 );
soldier.SetRotation(Math.PI/2, 0, 0);
soldier.SetPosition(-3.0, 0, 0);
soldier.SetAnimation("Unreal Take");
*/
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Create Street Fighter Ryu cmo model
//////////////////////////////////////////////////////////////////////////////////////////////////////////////

//var soldier = EntityManager.CreateEntity("fy4.cmo");

//soldier.SetScale(2, 2, -2 );
//soldier.SetRotation(0, Math.PI/2, 0);
//soldier.SetPosition(-5, 0, -1);
//soldier.SetAnimation("Take 001");

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//	Create Street Fighter Blanka cmo model
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
debugger;
var Ryu = EntityManager.CreateEntity("assets\\Ryu\\Ryu.sdkmesh");

Ryu.SetScale(2, 2, 2);
Ryu.SetRotation(0, Math.PI / 2, 0);
Ryu.SetPosition(-5, 0, -1);
Ryu.SetAnimation("Ryu");

Ryu.CreateAnimation("Ryu", "Spin Kick", 1, 83);
Ryu.CreateAnimation("Ryu", "Walk", 85, 15);
Ryu.CreateAnimation("Ryu", "Shoryuken", 100, 88);
Ryu.SetAnimation("Walk");

var seShoryuken = AudioEngine.CreateSoundEffect("assets\\Audio\\Ryu_Shoryuken.wav");
var seSpinKick = AudioEngine.CreateSoundEffect("assets\\Audio\\Ryu_SpinKick.wav");

//
//  Set input controller for player
//
Gamepad.OnMove = function (x, y) {
    var Sensitivity = 2.3;
    Ryu.SetVelocity(x * Sensitivity, 0, y * Sensitivity);
}

Gamepad.OnFire = function (index, p) {
    if (index == 0) {
        var anim = Ryu.SetAnimation("Spin Kick");
        seSpinKick.Play();

        //when the animation "Spin Kick" ends, play animation "walk"(idle) looping
        //Ryu.CreateAnimationEvent("Spin Kick", 82, function () {
        //    Ryu.SetAnimation("Walk");
        //});

        var p = new Promise(function (resolve) {
            Ryu.CreateAnimationEvent("Spin Kick", 82,resolve);           
        });

        p.then(() => {
            Ryu.SetAnimation("Shoryuken");
            //Play sound effect
            seShoryuken.Play();

            return new Promise(function (resolve) {
                Ryu.CreateAnimationEvent("Shoryuken", 87, resolve);
            });
        }).then(() => {
            Ryu.SetAnimation("Walk");
        });


    }

    if (index == 1) {
        var anim = Ryu.SetAnimation("Shoryuken");

        //when the animation "Shoryuken" ends, play animation "walk"(idle) looping
        Ryu.CreateAnimationEvent("Shoryuken", 87, function () {
            Ryu.SetAnimation("Walk");           
        });

        //Play sound effect
        seShoryuken.Play();
    }
}
//
// 
//
//Camera.Update = function() 
//{

//}

var blanka = EntityManager.CreateEntity("assets\\Blanka\\Blanka.sdkmesh");
blanka.name = "blanka";

blanka.SetScale(0.15, 0.15, 0.15);
blanka.SetRotation(0, Math.PI/2, 0);
blanka.SetPosition(5, 0, -1);
blanka.SetAnimation("Blanka");

var GirlWarrior = EntityManager.CreateEntity("assets\\GirlWarrior\\GirlWarrior.sdkmesh");
GirlWarrior.name = "GirlWarrior";

GirlWarrior.SetScale(0.02, 0.02, 0.02);
GirlWarrior.SetRotation(0, 0, 0);
GirlWarrior.SetPosition(3, 0, -3);
GirlWarrior.SetAnimation("GirlWarrior");

