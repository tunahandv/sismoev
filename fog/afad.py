"""AFAD verification for SismoEv.

When the ESP32 reports a local earthquake, the fog service calls `verify()`:
it asks AFAD for events within AFAD_RADIUS_KM of home in a time window around
the local detection and returns the best match (or None = "local vibration").

Run directly to see recent events near home:
    python afad.py            # last 24 hours
    python afad.py 72         # last 72 hours
"""
import math
import os
import sys
from datetime import datetime, timedelta, timezone

import requests

API = "https://deprem.afad.gov.tr/apiv2/event/filter"
HOME_LAT = float(os.getenv("HOME_LAT", "38.49"))
HOME_LON = float(os.getenv("HOME_LON", "27.70"))
RADIUS_KM = float(os.getenv("AFAD_RADIUS_KM", "200"))
TR = timezone(timedelta(hours=3))  # AFAD times are Turkey local time


def distance_km(lat1, lon1, lat2, lon2):
    r = 6371.0
    p1, p2 = math.radians(lat1), math.radians(lat2)
    dp, dl = p2 - p1, math.radians(lon2 - lon1)
    a = math.sin(dp / 2) ** 2 + math.cos(p1) * math.cos(p2) * math.sin(dl / 2) ** 2
    return 2 * r * math.asin(math.sqrt(a))


def fetch_events(start, end, radius_km=RADIUS_KM, min_mag=None):
    """Events between start and end (aware datetimes) within radius_km of home."""
    params = {
        "start": start.astimezone(TR).strftime("%Y-%m-%dT%H:%M:%S"),
        "end": end.astimezone(TR).strftime("%Y-%m-%dT%H:%M:%S"),
        "lat": HOME_LAT,
        "lon": HOME_LON,
        "maxrad": int(radius_km * 1000),  # metres
        "orderby": "timedesc",
        "format": "json",
    }
    if min_mag is not None:
        params["minmag"] = min_mag
    r = requests.get(API, params=params, timeout=10)
    r.raise_for_status()
    events = []
    for e in r.json():
        lat, lon = float(e.get("latitude", 0)), float(e.get("longitude", 0))
        events.append({
            "id": e.get("eventID"),
            "time": e.get("date"),
            "magnitude": float(e.get("magnitude", 0)),
            "type": e.get("type"),
            "depth_km": float(e.get("depth", 0)),
            "place": e.get("location") or ", ".join(
                x for x in (e.get("district"), e.get("province")) if x),
            "lat": lat,
            "lon": lon,
            "distance_km": round(distance_km(HOME_LAT, HOME_LON, lat, lon), 1),
        })
    return events


def verify(detected_at, before_min=2, after_min=8):
    """Best AFAD match for a local detection, or None.

    AFAD usually publishes a few minutes after the quake, so the fog service
    calls this again a few times (e.g. at +2, +5, +10 min) before giving up.
    """
    events = fetch_events(detected_at - timedelta(minutes=before_min),
                          detected_at + timedelta(minutes=after_min))
    if not events:
        return None
    # Stronger and closer quakes are felt more; pick the one most likely felt at home.
    return max(events, key=lambda e: e["magnitude"] - e["distance_km"] / 100)


if __name__ == "__main__":
    hours = float(sys.argv[1]) if len(sys.argv) > 1 else 24
    now = datetime.now(timezone.utc)
    events = fetch_events(now - timedelta(hours=hours), now)
    print(f"{len(events)} event(s) within {RADIUS_KM:.0f} km of home in the last {hours:g} h")
    for e in events:
        print(f"{e['time']}  M{e['magnitude']:.1f}  {e['depth_km']:.0f} km deep  "
              f"{e['distance_km']:.0f} km away  {e['place']}")
