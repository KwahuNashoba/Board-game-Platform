#include "BoardGamePluginPCH.h"
#include "Serializer.h"

void BG_Serializer::Serialize(VTypedObject *typedObj, VArchive& ar)
{
	if(ar.IsLoading())
	{
		//get list of objects variables
		VARIABLE_LIST const* const varList = typedObj->GetVariableList();
		VASSERT(varList);
		if(varList)
		{
			int const numVars = varList->GetSize();
			ar << numVars;

			for(VARIABLE_ELEM const* el = varList->first; el; el = el->next)
			{
				VisVariable_cl const* const var = el->value;
				ar << var->name;
				ar << var->type;

				switch(var->type)
				{
					case VULPTYPE_REFERENCED_OBJECT:
						{
							// cast entity objects address in char*(string) and add offset of this var in its objects class, then cast it to VTypedObjectReference
							//as far as I understood it :)
							VTypedObjectReference const* const ref = (VTypedObjectReference const*)((char const*)typedObj + var->clsOffset);
							//grab variable and put in archive
							ar << ref->GetReferencedObject();
						}
						break;
					default:
						{
							char valueString[1000];
							const_cast<VisVariable_cl*>(var)->GetValue(const_cast<VTypedObject*>((VTypedObject const*) typedObj), valueString);
							ar << valueString;
						}
				}
			}
		}
	}
	else
	{
		int numVars;
		ar >> numVars;
		for(int i = 0; i < numVars; i++)
		{
			VString varName;
			ar >> varName;

			int varType;
			ar >> varType;

			VisVariable_cl *const var = typedObj->GetVariable(varName);
			VASSERT(var && var->type == varType);
			if(var && var->type == varType)
			{
				switch(varType)
				{
					case VULPTYPE_REFERENCED_OBJECT:
						{
							VTypedObject *varObj;
							ar >> varObj;

							VTypedObjectReference *const ref = (VTypedObjectReference*)((char*)typedObj + var->clsOffset);
							ref->SetReferencedObject(varObj);
						}
						break;

					default:
						{
							VString varValue;
							ar >> varValue;

							var->SetValue((VTypedObject*)typedObj, varValue.AsChar());
						}
				}
			}
		}
	}
}
