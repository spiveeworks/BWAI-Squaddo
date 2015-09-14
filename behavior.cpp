
namespace Bhv {

class DevelopmentInterface {
public:
    //Asks if the development is completely finished, for better or for worse
    virtual bool isFinished () = 0;
    //Returns false if the development isn't finished or if it has succeeded, true if it has been terminated early.
    virtual bool hasFailed () = 0;
};

typedef DevelopmentInterface* Development;



class Execute: public DevelopmentInterface {
	Unit agent;
	UnitCommand task;
	Execute (UnitCommand task_c): task(task_c)
	{
		agent->issueCommand(task);
	}
	
	bool isFinished () {
		return agent->isIdle();
	}
	bool hasFailed () {return false;}
}; // IDKIDKIDKIDKIDKIDKIDKIDKIDKIDK

class BuildOrder: public DevelopmentInterface {
    UnitType build_type;
    Unit builder;
    TilePosition build_pos;
    
    bool reached_pos = false;
public:
    TerranConstruction (Unit who, UnitType what, TilePosition where): 
        build_type(what), 
        builder(who), 
        build_pos(where)
    {
        builder->build((Position)build_pos);
    }
    TerranConstruction () = delete;
    TerranConstruction (TerranConstruction const &base) = delete;
    
    void TravelCheck () {
        if (builder->isIdle())
        {
            reached_pos = true;
            builder->build(build_type, build_pos);
        }
    }
	
	Unit getBuilding() const {
		Unitset possible = Broodwar->getUnitsOnTile(build_pos, Filter::IsBuilding);
		auto ret = possible.begin();
		return ret == possible.end() ? nullptr : *ret;
	}
	
    bool hasFailed () const {
        return false; // that is, NYI
        if (builder->isConstructing())
            return false; // worker isn't trying to build something, but building hasn't been built.
		if (reached_pos) 
        return false;
    }
    bool isFinished () const {
        if (hasFailed()) return true;
        Unit building = getBuilding();
        if (!building)
            return false;//presumably hasn't been started yet
            
        if (building->getType() == build_type)
            return building->isCompleted();
        else
            return true;//should be caught by hasFailed
    }
};







}

