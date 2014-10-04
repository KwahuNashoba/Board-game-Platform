
g_characterProperties =
{
	m_idleFidgetMinWait = 10.0
	, m_idleFidgetMaxWait = 30.0
	, m_idleToMoveThreshold = 0.01
	
	, m_idleFidgetInterval = -1
	, m_idleTime = 0.0
}

function g_characterProperties:update()
	self.m_idleTime = self.m_idleTime + hkbGetTimestep();
end

function ShouldMove()
	local moveSpeed = hkbGetVariable("MoveSpeed");
	return (moveSpeed >= g_characterProperties.m_idleToMoveThreshold);
end

function ShouldFidget()
	if (g_characterProperties.m_idleFidgetInterval == -1) then
		g_characterProperties.m_idleFidgetInterval = math.random(g_characterProperties.m_idleFidgetMinWait, g_characterProperties.m_idleFidgetMaxWait);
	end
	if (hkbIsNodeActive("Idle_Fidget") == false and g_characterProperties.m_idleTime > g_characterProperties.m_idleFidgetInterval) then
		-- reset timer
		g_characterProperties.m_idleFidgetInterval = -1;
		g_characterProperties.m_idleTime = 0;
		return true;
	else
		return false;
	end
end

function ShouldDie()
	return (hkbGetVariable("Dying"));
end

function ShouldPlaySpawnAnimation()
	return (hkbGetVariable("Spawning"));
end

function ShouldChallenge()
	return (hkbGetVariable("Challenging"));
end

function ShouldHeal()
	return (hkbGetVariable("Healing"));
end

function ShouldMeleeAttack()
	return (hkbGetVariable("MeleeAttacking"));
end

function ShouldAoeAttack()
	return (hkbGetVariable("AoeAttacking"));
end

function ShouldRangedAttack()
	return (hkbGetVariable("RangedAttacking"));
end


-- /////////////////////////////////////////////////////////////////////////////
-- Idle State
-- /////////////////////////////////////////////////////////////////////////////
function onUpdateIdle()
	g_characterProperties:update();
	
	if (ShouldMove()) then
		hkbFireEvent("MoveStart");
	elseif (ShouldPlaySpawnAnimation()) then
		hkbFireEvent("SpawnStart");
	elseif (ShouldChallenge()) then
		hkbFireEvent("ChallengeStart");
	elseif (ShouldFidget()) then
		hkbFireEvent("IdleFidgetStart");
	elseif (ShouldHeal()) then
		hkbFireEvent("HealStart");
	elseif (ShouldMeleeAttack()) then
		hkbFireEvent("MeleeAttackStart");
	elseif (ShouldAoeAttack()) then
		hkbFireEvent("AoeStart");
	elseif (ShouldRangedAttack()) then
		hkbFireEvent("RangedAttackStart");
	end
end


-- /////////////////////////////////////////////////////////////////////////////
-- Move State
-- /////////////////////////////////////////////////////////////////////////////
function onUpdateMove()
	g_characterProperties:update();
	
	-- should we return to idle?
	if (ShouldMove() == false) then
		hkbFireEvent("MoveStop");
	elseif (ShouldPlaySpawnAnimation()) then
		hkbFireEvent("SpawnStart");
	elseif (ShouldChallenge()) then
		hkbFireEvent("ChallengeStart");
	elseif (ShouldFidget()) then
		hkbFireEvent("IdleFidgetStart");
	elseif (ShouldHeal()) then
		hkbFireEvent("HealStart");
	elseif (ShouldMeleeAttack()) then
		hkbFireEvent("MeleeAttackStart");
	elseif (ShouldAoeAttack()) then
		hkbFireEvent("AoeStart");
	elseif (ShouldRangedAttack()) then
		hkbFireEvent("RangedAttackStart");
	end
end


-- /////////////////////////////////////////////////////////////////////////////
-- MeleeAttack State
-- /////////////////////////////////////////////////////////////////////////////
function onActivateMeleeAttack()
	local chooseMeleeAttack = math.random(0, hkbGetVariable("MeleeChainCount"));
	hkbSetVariable("MeleeAttackType", chooseMeleeAttack);
end

function onHandleEventMeleeAttack()
	-- listen for the "MeleeAttackStop" event
	local eventName = hkbGetHandleEventName();
	if (eventName == "MeleeAttackStop") then
		hkbSetVariable("MeleeAttacking", false);
		hkbSetVariable("MeleeWeaponActive", false);
	end
	
	if (eventName == "MeleeWeaponActiveStart") then
		hkbSetVariable("MeleeWeaponActive", true);
	end
	
	if (eventName == "MeleeWeaponActiveEnd") then
		hkbSetVariable("MeleeWeaponActive", false);
	end
end


-- /////////////////////////////////////////////////////////////////////////////
-- Challenge State
-- /////////////////////////////////////////////////////////////////////////////
function onHandleEventChallenge()
	-- listen for the "ChallengeStop" event
	local eventName = hkbGetHandleEventName();
	if (eventName == "ChallengeStop") then
		hkbSetVariable("Challenging", false);
	end	
end


-- /////////////////////////////////////////////////////////////////////////////
-- Spawning State
-- /////////////////////////////////////////////////////////////////////////////
function onHandleEventSpawning()
	-- listen for the "SpawnEnd" event
	local eventName = hkbGetHandleEventName();
	if (eventName == "SpawnEnd") then
		hkbSetVariable("Spawning", false);
	end	
end


-- /////////////////////////////////////////////////////////////////////////////
-- Healing State
-- /////////////////////////////////////////////////////////////////////////////
function onHandleEventHealing()
	-- listen for the "ChallengeStop" event
	local eventName = hkbGetHandleEventName();
	if (eventName == "HealStop") then
		hkbSetVariable("Healing", false);
	end	
end


-- /////////////////////////////////////////////////////////////////////////////
-- Aoe Attacking State
-- /////////////////////////////////////////////////////////////////////////////
function onHandleEventAoeAttacking()
	-- listen for the "ChallengeStop" event
	local eventName = hkbGetHandleEventName();
	if (eventName == "AoeStop") then
		hkbSetVariable("AoeAttacking", false);
	end	
end


-- /////////////////////////////////////////////////////////////////////////////
-- Ranged Attacking State
-- /////////////////////////////////////////////////////////////////////////////
function onHandleEventRangedAttacking()
	-- listen for the "ChallengeStop" event
	local eventName = hkbGetHandleEventName();
	if (eventName == "RangedAttackStop") then
		hkbSetVariable("RangedAttacking", false);
	end	
end


-- /////////////////////////////////////////////////////////////////////////////
-- Alive SuperState
-- /////////////////////////////////////////////////////////////////////////////
function onActivateAlive()
end

function onUpdateAlive()
	if (ShouldDie()) then
		hkbFireEvent("DyingStart");
	end
end

function onDeactivateAlive()
end


-- /////////////////////////////////////////////////////////////////////////////
-- Dead SuperState
-- /////////////////////////////////////////////////////////////////////////////
function onActivateDead()
	-- @todo: is there a way to get the DeathType variable's max?
	local chooseDeath = math.random(0,2);	-- @todo: temp for testing
	hkbSetVariable("DeathType", chooseDeath);	-- Each character has its own DeathType max. values out of range will be ignored.
end

function onUpdateDead()
end

function onHandleEventDead()
	-- listen for the "DyingEnd" event
	local eventName = hkbGetHandleEventName();
	if (eventName == "DyingEnd") then
		hkbSetVariable("Dying", false);
	end	
end

function onDeactivateDead()
end
