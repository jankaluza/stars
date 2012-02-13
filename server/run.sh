#!/bin/sh

control_c() {
  kill `ps -A|grep stars|awk '{print $1}'`
  exit $?
}

trap control_c SIGINT

./unifier_service/stars_unifier_service &
./persistent_data_service/stars_persistent_data_service &
./frontend_service/stars_frontend_service &
./ticking_service/stars_ticking_service
