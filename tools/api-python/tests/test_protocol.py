import pytest
from pcb_tool.core.protocol import format_wxc_line, format_wxd_lines

def test_wxc_weekday_calculation():
    # 20 Marzo 2026 è Venerdì -> weekday 4 (0=Lun, 4=Ven)
    line = format_wxc_line("Trento", 15.0, 5.0, 40, 1, "2026-03-20T12:00:00")
    assert line.strip().endswith(",4")

def test_wxc_city_sanitization():
    # Verifica che virgole e nomi lunghi vengano gestiti
    line = format_wxc_line("Città, Molto Lunga Che Supera I Limiti", 10, 0, 0, 0, "2026-03-20T12:00:00")
    city_part = line.split(",")[1]
    assert "," not in city_part
    assert len(city_part) <= 23

def test_wxd_array_size():
    # 7 giorni x 24 ore = 168 entry
    temps = [20.0] * 168
    codes = [1] * 168
    lines = format_wxd_lines(temps, codes)
    assert len(lines) == 7
    assert lines[0].startswith("WXD,0")
