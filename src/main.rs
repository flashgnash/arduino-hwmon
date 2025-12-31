use std::{io::Write, thread, time::Duration};
use sysinfo::{Components, System};

fn read_gpu_busy() -> u8 {
    std::fs::read_dir("/sys/class/drm")
        .ok()
        .and_then(|dirs| {
            for d in dirs.flatten() {
                let p = d.path().join("device/gpu_busy_percent");
                if let Ok(s) = std::fs::read_to_string(p) {
                    if let Ok(v) = s.trim().parse::<u8>() {
                        return Some(v);
                    }
                }
            }
            None
        })
        .unwrap_or(0)
}

fn read_cpu_temp() -> f64 {
    let mut temps = Vec::new();

    if let Ok(paths) = std::fs::read_dir("/sys/class/hwmon/hwmon0") {
        for entry in paths.flatten() {
            let name = entry.file_name().into_string().unwrap_or_default();
            if name.starts_with("temp") && name.ends_with("_input") {
                if let Ok(content) = std::fs::read_to_string(entry.path()) {
                    if let Ok(t) = content.trim().parse::<f64>() {
                        temps.push(t / 1000.0);
                    }
                }
            }
        }
    }

    if temps.is_empty() {
        0.0
    } else {
        temps.iter().sum::<f64>() / temps.len() as f64
    }
}

fn main() {
    let mut sys = System::new_all();
    let mut comps = Components::new();

    let mut port = serialport::new("/dev/ttyACM0", 9600)
        .timeout(Duration::from_millis(100))
        .open()
        .unwrap();

    loop {
        sys.refresh_cpu_all();
        sys.refresh_memory();
        comps.refresh(false);

        let cpu = sys.global_cpu_usage() as u8;
        let mem = (sys.used_memory() * 100 / sys.total_memory()) as u8;
        let gpu = read_gpu_busy();

        let temp = read_cpu_temp().round();

        let out = format!(
            "CPU {:03} MEM {:03} TMP {:03} GPU {:03};",
            cpu, mem, temp, gpu
        );

        println!("{out}");

        port.write_all(out.as_bytes()).unwrap();
        thread::sleep(Duration::from_secs(1));
    }
}
