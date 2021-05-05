#include "pch.h"
#include "CNodeInstance.h"

#include "NodeMul.h"
#include "NodeSub.h"
#include "NodeCNodeTypePrint.h"
#include "NodeCNodeTypeStart.h"
#include "NodeCNodeTypeMathAdd.h"
#include "NodeGetObjectPos.h"
#include "NodeSetObjectPosition.h"
#include "NodeCeil.h"
#include "NodeFloor.h"
#include "NodeToRadians.h"
#include "NodeToDegrees.h"
#include "NodeLerp.h"
#include "NodeCos.h"
#include "NodeSin.h"
#include "NodeAbs.h"
#include "NodeAtan.h"
#include "NodeSetObjectScale.h"
#include "NodeForeach.h"
#include "NodeMouseInWorld.h"
#include "NodeLengthSqr.h"
#include "NodeGetPlayerPosition.h"
#include "NodeMergeVec3.h"
#include "NodeSplitVec3.h"
#include "NodeIf.h"
#include "NodeNot.h"
#include "NodeWithinRange.h"
#include "NodeSetTargetPosition.h"
#include "NodeOnTrigger.h"
#include "NodeSpawnEnemy.h"
#include "NodeTimer.h"
#include "NodeStore.h"
#include "NodeRetrieve.h"
#include "NodeGreater.h"
#include "NodeStopTimer.h"
#include "NodeChangeLevel.h"
#include "NodeStartCinematic.h"
#include "NodeGetPlayerID.h"
#include "NodeSeekTarget.h"
#include "NodeSetToIdle.h"
#include "NodeSetTarget.h"
#include "NodeAttackTarget.h"
#include "NodeWithinRangeOfSeekingFriends.h"
#include "NodeSpawnAbility.h"
#include "NodeWithinAttackRange.h"
#include "ForEachEnemyWithinRange.h"
#include "NodeSetObjectRotation.h"
#include "NodeOnButtonHit.h"
#include "NodeSetColliderStatus.h"
#include "NodeEnemySpawner.h"
#include "NodeGetMayaPos.h"
#include "NodeContinuousSpawner.h"
#include "NodeLoadLevel.h"
#include "NodeUnloadLevel.h"
#include "NodeInteractWith.h"
#include "NodeDialogue.h"
#include "NodeCameraOffset.h"
#include "NodeDialogOver.h"
#include "NodeChangeInteractableMesh.h"
#include "NodeGetInteractable.h"
#include "NodeMoveInteractable.h"
#include "NodeDayOver.h"

void CNodeTypeCollector::PopulateTypes()
{
	RegisterType<CNodeTypePrint>();
	RegisterType<CNodeTypeStart>();
	RegisterType<CNodeTypeMathAdd>();
	RegisterType<Mul>();
	RegisterType<Sub>();
	RegisterType<GetObjectPos>();
	RegisterType<SetObjectPosition>();
	RegisterType<Ceil>();
	RegisterType<Floor>();
	RegisterType<ToRadians>();
	RegisterType<ToDegrees>();
	RegisterType<Lerp>();
	RegisterType<Cos>();
	RegisterType<Sin>();
	RegisterType<Abs>();
	RegisterType<Atan>();
	RegisterType<SetObjectScale>();
	RegisterType<Foreach>();
	RegisterType<MouseInWorld>();
	RegisterType<LengthSqr>();
	RegisterType<GetPlayerPosition>();
	RegisterType<MergeVec3>();
	RegisterType<SplitVec3>();
	RegisterType<If>();
	RegisterType<Not>();
	RegisterType<WithinRange>();
	RegisterType<NodeSetTargetPosition>();
	RegisterType<NodeOnTrigger>();
	RegisterType<NodeSpawnEnemy>();
	RegisterType<NodeTimer>();
	RegisterType<NodeStore>();
	RegisterType<NodeRetrieve>();
	RegisterType<NodeGreater>();
	RegisterType<NodeStopTimer>();
	RegisterType<NodeChangeLevel>();
	RegisterType<NodeStartCinematic>();
	RegisterType<GetPlayerID>();
	RegisterType<NodeSeekTarget>();
	RegisterType<NodeSetToIdle>();
	RegisterType<NodeSetTarget>();
	RegisterType<NodeAttackTarget>();
	RegisterType<NodeWithinRangeOfSeekingFreinds>();
	RegisterType<NodeSpawnAbility>();
	RegisterType<NodeWithinAttackRange>();
	RegisterType<ForEachEnemyWithinRange>();
	RegisterType<NodeSetObjectRotation>();
	RegisterType<NodeOnButtonHit>();
	RegisterType<NodeSetColliderStatus>();
	RegisterType<NodeEnemySpawner>(); 
	RegisterType<NodeGetMayaPos>();
	RegisterType<ContinuousSpawner>();
	RegisterType<NodeLoadLevel>();
	RegisterType<NodeUnloadLevel>();
	RegisterType<NodeInteractWith>();
	RegisterType<NodeDialogue>();
	RegisterType<NodeCameraOffset>();
	RegisterType<NodeDialogOver>();
	RegisterType<NodeChangeInteractableMesh>();
	RegisterType<NodeGetInteractable>();
	RegisterType<NodeMoveInteractable>();
	RegisterType<NodeDayOver>();
}