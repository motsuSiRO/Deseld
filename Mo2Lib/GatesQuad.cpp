#include "GatesQuad.h"
#include "StateMachine.h"


const float INIT_MAX_MOVE_SPD = 1000.f;
const float INIT_MAX_VELOCITY = 100.f;
const float INIT_MASS = 5.f;
//const float FLOWER_HIT_SCALE = 200.f;


void GatesQuad::Update(float dt)
{

	fsm->Update(dt);

	Integrate(dt);
	collider[0].p = position;
}


class QUAD_Idle : public State<Enemy>
{
	float stak;
	void Begin(Enemy* e)
	{
		stak = 0.f;
		e->SetVelocity(Mo2Lib::Vec3(0.f, 0.f, 0.f));

		e->SetMAX_VELOCITY(INIT_MAX_VELOCITY);
		e->SetMAX_MOVE_SPEED(INIT_MAX_MOVE_SPD);
	}

	void Execute(Enemy* e)
	{
		stak += fsm->dt;
		if (1.f < stak)
		{
			fsm->ChangeState(QUAD_APPROACH);
		}
	}

	void End(Enemy* e)
	{

	}
};

class QUAD_Approach : public State<Enemy>
{
	float stak;
	void Begin(Enemy* e)
	{
		e->mass = INIT_MASS;
		stak = 0.f;

		e->SetMAX_VELOCITY(INIT_MAX_VELOCITY);
		e->SetMAX_MOVE_SPEED(INIT_MAX_MOVE_SPD);
	}

	void Execute(Enemy* e)
	{
		Mo2Lib::Float3 to_vec = e->target_pos - e->GetPos();

		float move_factor = e->GetMAX_MOVE_SPEED() * e->mass;
		e->AddForce(to_vec.GetNormalize() * move_factor);

		if (100.f > to_vec.LengthSq())
		{
			fsm->ChangeState(QUAD_IDLE);
		}

		stak += fsm->dt;
		if (3.f < stak)
		{
			fsm->ChangeState(QUAD_SIDEMOVE);
		}
	}

	void End(Enemy* e)
	{

	}
};

class QUAD_Sidemove : public State<Enemy>
{
	float stak;
	int isLeft;
	void Begin(Enemy* e)
	{
		stak = 0.f;
		isLeft = Mo2Lib::RandInt(0, 2);

		e->SetMAX_VELOCITY(INIT_MAX_VELOCITY);
		e->SetMAX_MOVE_SPEED(INIT_MAX_MOVE_SPD * 0.8f);

	}

	void Execute(Enemy* e)
	{
		Mo2Lib::Float3 to_vec = e->target_pos - e->GetPos();
		to_vec.Normalize();
		Mo2Lib::Float3 up_vec = { 0.f, 1.f, 0.f };

		Mo2Lib::Float3 right = to_vec.Cross(up_vec);


		float move_facter = e->GetMAX_MOVE_SPEED() * (1 - 2 * isLeft);
		e->AddForce(right.GetNormalize() * move_facter);

		stak += fsm->dt;
		if (0.8f < stak)
		{
			fsm->ChangeState(QUAD_APPROACH);
		}
	}

	void End(Enemy* e)
	{

	}
};

class QUAD_Bodyatk : public State<Enemy>
{
	void Begin(Enemy* e)
	{
		e->SetMAX_VELOCITY(INIT_MAX_VELOCITY);
		e->SetMAX_MOVE_SPEED(INIT_MAX_MOVE_SPD);
	}

	void Execute(Enemy* e)
	{

	}

	void End(Enemy* e)
	{

	}
};

class QUAD_Explosion : public State<Enemy>
{
	void Begin(Enemy* e)
	{
		e->SetMAX_VELOCITY(INIT_MAX_VELOCITY);
		e->SetMAX_MOVE_SPEED(INIT_MAX_MOVE_SPD);
	}

	void Execute(Enemy* e)
	{

	}

	void End(Enemy* e)
	{

	}
};


void GatesQuad::Init()
{
	fsm = std::make_unique<StateMachine<Enemy>>(this);
	std::shared_ptr<State<Enemy>> state;

	scale = { 2.f, 2.f, 2.f };
	collider.emplace_back(Mo2Lib::Sphere());
	collider.back().p = position;
	collider.back().r = 70.f;

	state = std::make_shared<QUAD_Idle>();
	fsm->AddState(QUAD_IDLE, state);
	state = std::make_shared<QUAD_Approach>();
	fsm->AddState(QUAD_APPROACH, state);
	state = std::make_shared<QUAD_Sidemove>();
	fsm->AddState(QUAD_SIDEMOVE, state);
	state = std::make_shared<QUAD_Bodyatk>();
	fsm->AddState(QUAD_BODYATK, state);
	state = std::make_shared<QUAD_Explosion>();
	fsm->AddState(QUAD_EXPLOSION, state);

	rotation = DirectX::XMQuaternionIdentity();


	fsm->SetCurrentState(QUAD_IDLE);
}
