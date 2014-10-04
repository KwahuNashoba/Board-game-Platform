
g_characterProperties =
{	
	m_idleFidgetMinWait = 10.0
	, m_idleFidgetMaxWait = 30.0
	, m_idleToMoveThreshold = 0.01
	
	, m_idleTime = 0.0
}

function g_characterProperties:update()
	self.m_idleTime = self.m_idleTime + hkbGetTimestep();
end

function ShouldMove()
	return (hkbGetVariable("MoveSpeed") >= g_characterProperties.m_idleToMoveThreshold);
end

function ShouldPlaySpawnAnimation()
	return (hkbGetVariable("Spawning"));
end

function ShouldMeleeAttack()
	return (hkbGetVariable("MeleeAttacking"));
end

function ShouldPowerAttack()
	return (hkbGetVariable("MeleePowerAttacking"));
end

function ShouldAoeAttack()
	return (hkbGetVariable("AoeAttacking"));
end

function ShouldRangedAttack()
	return (hkbGetVariable("RangedAttacking"));
end

function ShouldFidget()
	local randomFidgetInterval = math.random(g_characterProperties.m_idleFidgetMinWait, g_characterProperties.m_idleFidgetMaxWait);
	return (hkbIsNodeActive("Idle_Fidget") == false and g_characterProperties.m_idleTime >= randomFidgetInterval);
end

function DoFidget()
	-- choose my fidget
	local whichFidget = math.random(1,3);
	if (whichFidget == 1) then
		hkbFireEvent("IdleFidgetStart01");
	elseif (whichFidget == 2) then
		hkbFireEvent("IdleFidgetStart02");
	else
		hkbFireEvent("IdleFidgetStart03");
	end
		
	g_characterProperties.m_idleTime = 0;
end

function ShouldDie()
	return (hkbGetVariable("Dying"));
end

-- /////////////////////////////////////////////////////////////////////////////
-- Idle State
-- /////////////////////////////////////////////////////////////////////////////
function onActivateIdle()
	g_characterProperties.m_idleTime = 0;
end

function onUpdateIdle()	
	if (ShouldMove()) then
		hkbFireEvent("MoveStart");
	elseif (ShouldFidget()) then
		DoFidget();
	elseif (ShouldPlaySpawnAnimation()) then
		hkbFireEvent("SpawnStart");
	elseif (ShouldMeleeAttack()) then
		hkbFireEvent("MeleeAttackStart");
	elseif (ShouldPowerAttack()) then
		hkbFireEvent("PowerAttackStart");
	elseif (ShouldAoeAttack()) then
		hkbFireEvent("AoeAttackStart");
	elseif (ShouldRangedAttack()) then
		hkbFireEvent("RangedAttackStart");
	end
end


-- /////////////////////////////////////////////////////////////////////////////
-- Move State
-- /////////////////////////////////////////////////////////////////////////////
function onUpdateMove()	
	-- should we return to idle?
	if (ShouldMove() == false) then
		hkbFireEvent("MoveStop");
	elseif (ShouldPlaySpawnAnimation()) then
		hkbFireEvent("SpawnStart");
	elseif (ShouldMeleeAttack()) then
		hkbFireEvent("MeleeAttackStart");
	elseif (ShouldPowerAttack()) then
		hkbFireEvent("PowerAttackStart");
	elseif (ShouldAoeAttack()) then
		hkbFireEvent("AoeAttackStart");
	elseif (ShouldRangedAttack()) then
		hkbFireEvent("RangedAttackStart");
	end
end


-- /////////////////////////////////////////////////////////////////////////////
-- MeleeAttack States
-- /////////////////////////////////////////////////////////////////////////////
function onHandleEventMeleeAttack()
	-- listen for the "MeleeAttackEnd" event
	local eventName = hkbGetHandleEventName();
	if (eventName == "MeleeAttackEnd") then
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

function onHandleEventMeleePowerAttack()
	-- listen for the "PowerAttackEnd" event
	local eventName = hkbGetHandleEventName();
	if (eventName == "PowerAttackEnd") then
		hkbSetVariable("MeleePowerAttacking", false);
		hkbSetVariable("MeleeWeaponActive", false);
	end
	
	if (eventName == "MeleeWeaponActiveStart") then
		hkbSetVariable("MeleeWeaponActive", true);
	end
	
	if (eventName == "MeleeWeaponActiveEnd") then
		hkbSetVariable("MeleeWeaponActive", false);
	end
end

function onHandleEventAoeAttack()
	-- listen for the "AoeAttackEnd" event
	local eventName = hkbGetHandleEventName();
	if (eventName == "AoeAttackEnd") then
		hkbSetVariable("AoeAttacking", false);
		hkbSetVariable("MeleeWeaponActive", false);
	end
	
	if (eventName == "MeleeWeaponActiveStart") then
		hkbSetVariable("MeleeWeaponActive", true);
	end
	
	if (eventName == "MeleeWeaponActiveEnd") then
		hkbSetVariable("MeleeWeaponActive", false);
	end
end

function onHandleEventRangedAttack()
	-- listen for the "RangedAttackEnd" event
	local eventName = hkbGetHandleEventName();
	if (eventName == "RangedAttackEnd") then
		hkbSetVariable("RangedAttacking", false);
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
-- Alive SuperState
-- /////////////////////////////////////////////////////////////////////////////
function onUpdateAlive()
	g_characterProperties:update();
	
	if (ShouldDie()) then
		hkbFireEvent("DyingStart");
	end
end


-- /////////////////////////////////////////////////////////////////////////////
-- Dead SuperState
-- /////////////////////////////////////////////////////////////////////////////
function onUpdateDead()
	if (hkbGetVariable("Respawning")) then
		hkbFireEvent("Respawn");
		hkbSetVariable("Respawning", false);
	end	
end

function onHandleEventDead()
	-- listen for the "DyingEnd" event
	local eventName = hkbGetHandleEventName();
	if (eventName == "DyingEnd") then
		hkbSetVariable("Dying", false);
	end	
end
