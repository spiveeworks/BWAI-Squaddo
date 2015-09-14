
typedef UnitInterface const* ImmutableUnit;
// ImmutableUnit cannot call non-const methods, meaning that passing ImmutableUnit to a function ensures that the unit 
    // will not be given instructions or orders
// Unit can always cast to ImmutableUnit due to the nature of const-casting, but not the other way round
// 'ImmutableUnit' would be called ConstUnit, however ConstUnit is a very confusing name 
    // because ConstUnit would give a different effect to const Unit, 
	// and the very point of this typedef is to remove the need for that distinction

struct IsFromSet {
	Unitset const &test_set;
	IsFromSet (Unitset const &test_set_c): test_set(test_set_c)
	{
	}
	bool operator() (Unit test_for) const 
	{
		return test_set.count(test_for);
	}
};

class UnitDistributor {
	
	//Asks for a unit of the given type. nullptr indicates that the offer was denied (
	virtual Unit popUnit (UnitType req_type) = 0;
	
	//Asks for any or perhaps the best unit. Used for a default case, but since an implementation might need this information, assume this function won't always indicate unavailibility.
	virtual Unit popUnit () = 0;
	
	// Maybe this would be good later?? 
	//Unit popUnit (UnitFilter) = 0;
	
	//Passes a unit into the node. 
	//false garantees that the unit was not modified
	//true garantees that the object is now keeping track of the Unit, and removing the Unit from other lists won't result in the unit being forgotten
	virtual bool pushUnit (Unit offer) = 0;
	
	//Tells whether pushUnit would accept a particular unit.
	//Garantees that pushUnit would execute as described if called.
	virtual bool testUnit (ImmutableUnit offer) const = 0;
};

//This class will be generalized into AbstractMine eventually, with the terran-specific parts being in a sub-class
class TerranMine/*: public UnitDistributor*/ {
private:
	//Command 
    Unit RC_unit;
	Unitset workers;
	Unitset idle;
	Unitset minerals;
	Unitset geysers;
	
protected:
	// puts to work or adds to idle if it can't work.
	// later will balance minerals and vespene
	// does not check that the unit actually is a worker
	void Assign(Unit worker)
	{
		if (worker->isCompleted())
		{
			Unit mineral = worker->getClosestUnit(IsFromSet(minerals));
			if (mineral)
				if (worker->gather(mineral))
				{
					workers.insert(worker);
					return;
				}
		}
		//will be called if any of the above tests fail,
		//if the worker is still training, or there are no minerals, or if broodwar rejects the order for some other reason
		idle.insert(worker);
	}
	
	bool isTraining()
	{
		return RC_unit->isTraining();
	}
	
	bool isUnsaturated() const {return true;} // ALWAYS ACCEPT THE WORKERS
	
public:
    static int const MINE_RANGE = 512;


	void FlushStragglers()
	{
		Unitset stragglers = move(idle);
		idle.clear();
		for (auto &W: stragglers)
			Assign(W);
	}
	
	Unit getDepot () const {return RC_unit;}
	
	Unit popUnit (UnitType req_type)
	{
		if (req_type.isWorker())
			return popUnit();
		else return nullptr;
	}
	Unit popUnit ()
	{
		if (workers.empty())
			return nullptr;
		Unit ret = *workers.begin();
		workers.erase(ret);
		return ret;
	}
	
	bool pushUnit (Unit offer) 
	{
		if ((offer->getType().isWorker()) && isUnsaturated())
            Assign(offer);
		else 
			return false;
        return true;
	}
    /*
	bool testUnit (ImmutableUnit offer) const
	{
		return (offer->getType().isMineralField()) || (offer->getType().isWorker()) && isUnsaturated();
	}*/
	
	TerranMine(Unit RC_unit_c): RC_unit(RC_unit_c)
	{
        minerals = RC_unit->getUnitsInRadius(MINE_RANGE, Filter::IsMineralField); // issue: use regions for better management without units thinking walls don't exist.
	}
};


class TerranBase {
    
public:
    TerranMine mine;
    TerranBase (Unit RD_unit_c): mine(RD_unit_c)
    {
    }
    
    
    
    
};



