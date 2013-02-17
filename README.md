Jump Planner
============

This is a small thing i wrote for fun. Its an implementation of the A\* algorithm to calculate jump paths in the Eve Online universe.

Requirements
------------

Postgresql database with the data dump loaded 

The following view should be created:

```sql
CREATE VIEW jumpdistances AS
    SELECT fromss.solarsystemid AS from_solarsystemid, fromss.security AS from_security, toss.solarsystemid AS to_solarsystemid, toss.security AS to_security, (sqrt(((((toss.x - fromss.x) * (toss.x - fromss.x)) + ((toss.y - fromss.y) * (toss.y - fromss.y))) + ((toss.z - fromss.z) * (toss.z - fromss.z)))) / ((9460000000000000::bigint)::numeric)::double precision) AS distance FROM (public.mapsolarsystems fromss JOIN public.mapsolarsystems toss ON ((fromss.solarsystemid <> toss.solarsystemid)));
```


Compilation requirements
------------------------

* libpq (8.4)
* gsoap (2.7)

Notice
------
EVE Online and the EVE logo are the registered trademarks of CCP hf. All rights are reserved worldwide.
