#include "Turnout.h"

String Turnout::toJson() const {
  StaticJsonDocument<200> doc;
  doc["id"] = id;
  doc["address"] = address;
  doc["closedEndpoint"] = closedEndpoint;
  doc["openEndpoint"] = openEndpoint;
  doc["reversed"] = reversed;
  doc["testInProgress"] = testInProgress;

  String jsonString;
  serializeJson(doc, jsonString);
  return jsonString;
}

Turnout Turnout::fromJson(const JsonObject& json) {
  return Turnout(
    json["id"],
    json["address"],
    json["closedEndpoint"],
    json["openEndpoint"],
    json["reversed"],
    json["testInProgress"]
  );
}
