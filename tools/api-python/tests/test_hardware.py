import os
from pcb_tool.hardware.storage import read_gps_coords

def test_gps_reading_mock(tmp_path):
    # Crea un file gps_last.csv finto nella cartella temporanea
    d = tmp_path / "gps_last.csv"
    d.write_text("46.06,11.12")
    
    coords = read_gps_coords(str(tmp_path))
    assert coords == (46.06, 11.12)
