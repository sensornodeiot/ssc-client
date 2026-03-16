#pragma once

const char PORTAL_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 Configuration — Sensornode</title>
<style>
  :root {
    --blue:       #0047FF;
    --blue-mid:   #25A0FF;
    --blue-light: #E9EBFF;
    --bg:         #F0F4FF;
    --white:      #FFFFFF;
    --dark:       #1A1A2E;
    --mid:        #4A5068;
    --light:      #8A93B2;
    --border:     #DDE3F5;
    --red:        #EF4444;
    --green:      #10B981;
    --radius:     10px;
    --radius-sm:  6px;
    --shadow:     0 1px 4px rgba(0,71,255,0.08);
  }

  *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }

  body {
    font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, sans-serif;
    background: var(--bg);
    color: var(--dark);
    min-height: 100vh;
    font-size: 14px;
  }

  header {
    background: var(--white);
    border-bottom: 1px solid var(--border);
    position: sticky;
    top: 0;
    z-index: 100;
  }
  .header-inner {
    max-width: 680px;
    margin: 0 auto;
    padding: 0 20px;
  }
  .brand-row {
    display: flex;
    align-items: center;
    justify-content: space-between;
    padding: 13px 0 11px;
    border-bottom: 1px solid var(--border);
  }
  .sn-logo {
    display: flex;
    align-items: center;
    gap: 9px;
    text-decoration: none;
  }
  .sn-wordmark { display: flex; flex-direction: column; line-height: 1; }
  .sn-wordmark .name {
    font-size: 13px; font-weight: 700;
    color: var(--blue); letter-spacing: -0.2px;
    font-family: 'Courier New', monospace;
  }
  .sn-wordmark .sub {
    font-size: 8.5px; letter-spacing: 1.4px;
    color: var(--light); text-transform: uppercase; margin-top: 2px;
  }
  .device-badge {
    display: flex; align-items: center; gap: 6px;
    font-size: 11px; color: var(--light);
  }
  .dot { width: 6px; height: 6px; border-radius: 50%; background: var(--red); flex-shrink: 0; }
  .dot.online { background: var(--green); }

  .nav-tabs { display: flex; }
  .tab {
    flex: 1;
    display: flex; align-items: center; justify-content: center; gap: 5px;
    padding: 10px 6px;
    font-size: 12px; font-weight: 500; color: var(--light);
    cursor: pointer;
    border-bottom: 2px solid transparent;
    transition: color 0.15s, border-color 0.15s;
    user-select: none; white-space: nowrap;
  }
  .tab:hover { color: var(--blue); }
  .tab.active { color: var(--blue); font-weight: 600; }
  .tab.done { color: var(--green); }
  .tab.done .step-num { background: var(--green); }
  .step-num {
    width: 16px; height: 16px; border-radius: 50%;
    background: var(--border); color: var(--white);
    font-size: 9px; font-weight: 700;
    display: flex; align-items: center; justify-content: center;
    flex-shrink: 0; transition: background 0.2s;
  }
  .tab.active .step-num { background: var(--blue); }

  .step-progress {
    height: 3px;
    background: var(--border);
    position: relative;
    overflow: hidden;
  }
  .step-progress-fill {
    height: 100%;
    background: linear-gradient(90deg, var(--blue), var(--blue-mid));
    transition: width 0.35s ease;
    width: 0%;
  }

  main {
    max-width: 680px;
    margin: 0 auto;
    padding: 28px 20px 56px;
  }

  .page { display: none; }
  .page.active { display: block; }

  .page-title { margin-bottom: 20px; }
  .page-title h1 { font-size: 19px; font-weight: 700; letter-spacing: -0.3px; }
  .page-title p  { font-size: 12px; color: var(--light); margin-top: 3px; }
  .accent-line { width: 32px; height: 3px; background: var(--blue); border-radius: 2px; margin-top: 8px; }

  .step-label {
    display: inline-flex; align-items: center; gap: 6px;
    font-size: 11px; font-weight: 600; color: var(--blue);
    background: var(--blue-light); border-radius: 20px;
    padding: 3px 10px; margin-bottom: 10px;
  }

  .hero {
    background: linear-gradient(135deg, var(--blue) 0%, #1469FF 55%, var(--blue-mid) 100%);
    border-radius: var(--radius);
    padding: 18px 22px; color: #fff;
    margin-bottom: 14px;
    position: relative; overflow: hidden;
  }
  .hero::after {
    content: ''; position: absolute;
    right: -18px; top: -18px;
    width: 110px; height: 110px;
    border-radius: 50%;
    background: rgba(255,255,255,0.06);
    pointer-events: none;
  }
  .hero h2 { font-size: 15px; font-weight: 700; position: relative; }
  .hero p  { font-size: 12px; opacity: .75; margin-top: 3px; position: relative; }

  .alert {
    display: flex; align-items: flex-start; gap: 9px;
    padding: 11px 14px; border-radius: var(--radius-sm);
    font-size: 12px; line-height: 1.5; margin-bottom: 18px;
  }
  .alert svg { flex-shrink: 0; margin-top: 1px; }
  .alert-info { background: var(--blue-light); border: 1px solid #C7D2FE; color: #3730A3; }
  .alert-warn { background: #FFFBEB; border: 1px solid #FDE68A; color: #92400E; }
  .alert-success { background: #D1FAE5; border: 1px solid #6EE7B7; color: #065F46; }
  .alert-error { background: #FEE2E2; border: 1px solid #FECACA; color: #991B1B; }

  .card {
    background: var(--white);
    border: 1px solid var(--border);
    border-radius: var(--radius);
    box-shadow: var(--shadow);
  }
  .card + .card { margin-top: 14px; }
  .card-header {
    padding: 14px 20px;
    border-bottom: 1px solid var(--border);
    display: flex; align-items: center; justify-content: space-between;
  }
  .card-title {
    font-size: 13px; font-weight: 600; color: var(--dark);
    display: flex; align-items: center; gap: 7px;
  }
  .card-title svg { color: var(--blue); }
  .card-body { padding: 20px; }
  .card-footer {
    padding: 14px 20px;
    border-top: 1px solid var(--border);
    display: flex; gap: 8px; align-items: center;
  }

  .grid {
    display: grid;
    grid-template-columns: repeat(auto-fit, minmax(220px, 1fr));
    gap: 14px;
  }

  .info-row {
    display: flex; justify-content: space-between; align-items: center;
    padding: 11px 20px;
    border-bottom: 1px solid var(--border);
    font-size: 13px; gap: 12px;
  }
  .info-row:last-child { border-bottom: none; }
  .info-row .k { color: var(--light); white-space: nowrap; }
  .info-row .v { font-weight: 500; text-align: right; word-break: break-all; }
  .info-row .v.empty { color: var(--light); font-style: italic; font-weight: 400; }

  .badge {
    display: inline-flex; align-items: center; gap: 4px;
    padding: 3px 8px; border-radius: 20px;
    font-size: 11px; font-weight: 600;
  }
  .badge-red   { background: #FEE2E2; color: var(--red); }
  .badge-green { background: #D1FAE5; color: #059669; }

  .checklist { padding: 16px 20px; display: flex; flex-direction: column; gap: 9px; }
  .check-item { display: flex; align-items: center; gap: 10px; font-size: 13px; color: var(--mid); }
  .check-icon {
    width: 20px; height: 20px; border-radius: 50%;
    display: flex; align-items: center; justify-content: center; flex-shrink: 0;
  }
  .check-icon.fail { background: #FEE2E2; }
  .check-icon.pass { background: #D1FAE5; }

  .form-group { margin-bottom: 16px; }
  .form-group:last-child { margin-bottom: 0; }
  .label {
    display: block; font-size: 12px; font-weight: 600;
    color: var(--mid); margin-bottom: 5px;
  }
  .label .req { color: var(--red); margin-left: 2px; }
  .hint { font-size: 11px; color: var(--light); margin-top: 4px; }

  .input-wrap { position: relative; }
  .input-wrap input { padding-right: 38px; }
  .toggle-pw {
    position: absolute; right: 10px; top: 50%; transform: translateY(-50%);
    background: none; border: none; cursor: pointer;
    color: var(--light); padding: 2px; display: flex; align-items: center;
    transition: color 0.15s;
  }
  .toggle-pw:hover { color: var(--blue); }

  input[type=text],
  input[type=password],
  input[type=number] {
    width: 100%; padding: 9px 12px;
    border: 1px solid var(--border); border-radius: var(--radius-sm);
    font-size: 13px; color: var(--dark); background: var(--white);
    font-family: inherit; outline: none;
    transition: border-color 0.15s, box-shadow 0.15s;
  }
  input:focus { border-color: var(--blue); box-shadow: 0 0 0 3px rgba(0,71,255,0.10); }
  input::placeholder { color: var(--light); }
  input.invalid { border-color: var(--red); }
  input.invalid:focus { box-shadow: 0 0 0 3px rgba(239,68,68,0.12); }
  .field-error { font-size: 11px; color: var(--red); margin-top: 4px; display: none; }
  .field-error.show { display: block; }

  .btn {
    display: inline-flex; align-items: center; gap: 6px;
    padding: 9px 18px; border-radius: var(--radius-sm);
    font-size: 13px; font-weight: 600;
    cursor: pointer; border: none; font-family: inherit;
    transition: all 0.15s;
  }
  .btn-primary  { background: var(--blue); color: #fff; }
  .btn-primary:hover { background: #0038D4; box-shadow: 0 4px 12px rgba(0,71,255,0.28); transform: translateY(-1px); }
  .btn-outline  { background: transparent; color: var(--mid); border: 1px solid var(--border); }
  .btn-outline:hover { border-color: var(--blue); color: var(--blue); }
  .btn-ghost    { background: transparent; color: var(--light); border: none; padding: 9px 12px; }
  .btn-ghost:hover { color: var(--blue); }
  .btn:disabled { opacity: .4; cursor: not-allowed; transform: none !important; box-shadow: none !important; }

  footer {
    border-top: 1px solid var(--border);
    background: var(--white);
  }
  .footer-inner {
    max-width: 680px;
    margin: 0 auto;
    padding: 12px 20px;
    display: flex;
    align-items: center;
    justify-content: space-between;
    font-size: 11px;
    color: var(--light);
  }

  .upload-zone {
    border: 2px dashed var(--border); border-radius: var(--radius);
    padding: 36px 20px; text-align: center; cursor: pointer;
    transition: border-color 0.2s, background 0.2s;
    background: rgba(0,71,255,0.02);
  }
  .upload-zone:hover, .upload-zone.drag { border-color: var(--blue); background: rgba(0,71,255,0.04); }
  .upload-icon-wrap {
    width: 46px; height: 46px; background: var(--blue-light);
    border-radius: 50%; display: flex; align-items: center; justify-content: center;
    margin: 0 auto 12px; color: var(--blue);
  }
  .upload-zone h3 { font-size: 13px; font-weight: 600; }
  .upload-zone p  { font-size: 12px; color: var(--light); margin-top: 3px; }

  .progress-wrap { margin-top: 14px; }
  .progress-meta { display: flex; justify-content: space-between; font-size: 12px; margin-bottom: 5px; }
  .progress-meta .fname { color: var(--mid); font-weight: 500; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; max-width: 76%; }
  .progress-meta .pct { color: var(--blue); font-weight: 700; }
  .progress-bar { background: var(--blue-light); border-radius: 20px; height: 5px; overflow: hidden; }
  .progress-fill { height: 100%; background: linear-gradient(90deg, var(--blue), var(--blue-mid)); border-radius: 20px; width: 0%; transition: width 0.3s ease; }

  .success-banner {
    display: none;
    flex-direction: column; align-items: center; text-align: center;
    padding: 28px 20px; gap: 10px;
  }
  .success-banner.show { display: flex; }
  .success-icon {
    width: 52px; height: 52px; background: #D1FAE5;
    border-radius: 50%; display: flex; align-items: center; justify-content: center;
    color: #059669;
  }
  .success-banner h3 { font-size: 15px; font-weight: 700; color: var(--dark); }
  .success-banner p  { font-size: 12px; color: var(--light); }

  .status-msg {
    padding: 10px 14px;
    border-radius: var(--radius-sm);
    font-size: 12px;
    margin-bottom: 14px;
    display: none;
  }
  .status-msg.show { display: block; }
  .status-msg.success { background: #D1FAE5; color: #065F46; border: 1px solid #6EE7B7; }
  .status-msg.error { background: #FEE2E2; color: #991B1B; border: 1px solid #FECACA; }

  .reboot-modal {
    display: none;
    position: fixed;
    top: 0; left: 0; right: 0; bottom: 0;
    background: rgba(0,0,0,0.5);
    z-index: 200;
    align-items: center;
    justify-content: center;
  }
  .reboot-modal.show { display: flex; }
  .reboot-modal-content {
    background: var(--white);
    padding: 24px;
    border-radius: var(--radius);
    max-width: 320px;
    text-align: center;
  }
  .reboot-modal h3 { margin-bottom: 8px; }
  .reboot-modal p { font-size: 13px; color: var(--mid); margin-bottom: 16px; }
  .reboot-modal .btn-row { display: flex; gap: 8px; justify-content: center; }
</style>
</head>
<body>

<header>
  <div class="header-inner">
    <div class="brand-row">
      <a class="sn-logo" href="#">
        <svg width="32" height="32" viewBox="0 0 32 32" fill="none">
          <rect width="32" height="32" rx="8" fill="#0047FF"/>
          <path d="M10 13C10 13 12.2 10 16 10C19.8 10 22 13 22 13" stroke="white" stroke-width="1.8" stroke-linecap="round"/>
          <path d="M7.5 10.5C7.5 10.5 11 6 16 6C21 6 24.5 10.5 24.5 10.5" stroke="white" stroke-width="1.8" stroke-linecap="round" opacity="0.5"/>
          <circle cx="16" cy="13.5" r="1.8" fill="white"/>
          <line x1="16" y1="15.3" x2="16" y2="24" stroke="white" stroke-width="1.8" stroke-linecap="round"/>
          <line x1="16" y1="24" x2="11.5" y2="27" stroke="white" stroke-width="1.8" stroke-linecap="round"/>
          <line x1="16" y1="24" x2="20.5" y2="27" stroke="white" stroke-width="1.8" stroke-linecap="round"/>
          <line x1="13" y1="22.5" x2="19" y2="22.5" stroke="white" stroke-width="1.4" stroke-linecap="round"/>
        </svg>
        <div class="sn-wordmark">
          <span class="name">sensornode</span>
          <span class="sub">IoT Technology</span>
        </div>
      </a>
      <div class="device-badge">
        <span class="dot" id="status-dot"></span>
        <span id="status-label">ESP32 · Loading...</span>
      </div>
    </div>

    <nav class="nav-tabs">
      <div class="tab active" id="tab-status" onclick="goToPage('status')">
        <span class="step-num">1</span>Status
      </div>
      <div class="tab" id="tab-wifi" onclick="goToPage('wifi')">
        <span class="step-num">2</span>WiFi
      </div>
      <div class="tab" id="tab-mqtt" onclick="goToPage('mqtt')">
        <span class="step-num">3</span>MQTT
      </div>
      <div class="tab" id="tab-ota" onclick="goToPage('ota')">
        <span class="step-num">4</span>OTA
      </div>
    </nav>

    <div class="step-progress">
      <div class="step-progress-fill" id="step-bar"></div>
    </div>
  </div>
</header>

<main>

  <!-- STATUS PAGE -->
  <div class="page active" id="page-status">
    <div class="page-title">
      <div class="step-label">Step 1 of 4</div>
      <h1>Device Status</h1>
      <p>Review your current configuration before starting setup</p>
      <div class="accent-line"></div>
    </div>

    <div class="hero" id="hero-banner">
      <h2 id="hero-title">Loading configuration...</h2>
      <p id="hero-text">Please wait while we load the device settings.</p>
    </div>

    <div class="grid">
      <div class="card">
        <div class="card-header">
          <div class="card-title">Current Configuration</div>
          <span class="badge badge-red" id="cfg-badge">Incomplete</span>
        </div>
        <div class="info-row"><span class="k">WiFi SSID</span><span class="v empty" id="cfg-ssid">Not configured</span></div>
        <div class="info-row"><span class="k">MQTT Host</span><span class="v empty" id="cfg-host">Not configured</span></div>
        <div class="info-row"><span class="k">MQTT Port</span><span class="v" id="cfg-port">1883</span></div>
        <div class="info-row"><span class="k">Tenant ID</span><span class="v empty" id="cfg-tenant">Not configured</span></div>
        <div class="info-row"><span class="k">Application ID</span><span class="v empty" id="cfg-app">Not configured</span></div>
        <div class="info-row"><span class="k">Device ID</span><span class="v empty" id="cfg-device">Not configured</span></div>
      </div>

      <div class="card">
        <div class="card-header">
          <div class="card-title">Setup Checklist</div>
        </div>
        <div class="checklist">
          <div class="check-item" id="chk-wifi">
            <div class="check-icon fail" id="chk-wifi-icon"></div>
            WiFi credentials
          </div>
          <div class="check-item" id="chk-mqtt">
            <div class="check-icon fail" id="chk-mqtt-icon"></div>
            MQTT broker connection
          </div>
          <div class="check-item" id="chk-device">
            <div class="check-icon fail" id="chk-device-icon"></div>
            Device identity (ID + Token)
          </div>
        </div>
      </div>
    </div>

    <div style="display:flex;gap:10px;margin-top:16px;justify-content:center;">
      <button class="btn btn-primary" onclick="goToPage('wifi')">Start Setup</button>
      <button class="btn btn-outline" onclick="showRebootModal()">Reboot</button>
    </div>
  </div>

  <!-- WIFI PAGE -->
  <div class="page" id="page-wifi">
    <div class="page-title">
      <div class="step-label">Step 2 of 4</div>
      <h1>WiFi Settings</h1>
      <p>Connect this device to a wireless network</p>
      <div class="accent-line"></div>
    </div>

    <div class="status-msg" id="wifi-status"></div>

    <div class="card">
      <div class="card-header">
        <div class="card-title">Network Credentials</div>
      </div>
      <div class="card-body">
        <div class="grid">
          <div class="form-group">
            <label class="label">WiFi SSID <span class="req">*</span></label>
            <input type="text" id="wifi-ssid" placeholder="e.g. MyNetwork_5G" maxlength="32">
            <div class="field-error" id="err-wifi-ssid">WiFi SSID is required</div>
          </div>
          <div class="form-group">
            <label class="label">WiFi Password <span class="req">*</span></label>
            <div class="input-wrap">
              <input type="password" id="wifi-pass" placeholder="Enter password" maxlength="64">
              <button type="button" class="toggle-pw" onclick="togglePw('wifi-pass', this)">
                <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" class="eye-icon"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3"/></svg>
              </button>
            </div>
            <div class="field-error" id="err-wifi-pass">Password is required</div>
          </div>
        </div>
      </div>
      <div class="card-footer">
        <button class="btn btn-primary" id="btn-save-wifi" onclick="saveWifi()">Save & Continue</button>
        <button class="btn btn-ghost" onclick="goToPage('status')">Back</button>
        <button class="btn btn-outline" onclick="goToPage('mqtt')" style="margin-left:auto;">Skip</button>
      </div>
    </div>
  </div>

  <!-- MQTT PAGE -->
  <div class="page" id="page-mqtt">
    <div class="page-title">
      <div class="step-label">Step 3 of 4</div>
      <h1>MQTT Settings</h1>
      <p>Configure the Sensorclouds broker connection</p>
      <div class="accent-line"></div>
    </div>

    <div class="status-msg" id="mqtt-status"></div>

    <div class="card">
      <div class="card-header">
        <div class="card-title">Broker & Identity</div>
      </div>
      <div class="card-body">
        <div class="grid">
          <div class="form-group">
            <label class="label">MQTT Host <span class="req">*</span></label>
            <input type="text" id="mqtt-host" placeholder="mqtt.sensornodeiot.com" maxlength="64">
            <div class="field-error" id="err-mqtt-host">MQTT Host is required</div>
          </div>
          <div class="form-group">
            <label class="label">MQTT Port <span class="req">*</span></label>
            <input type="number" id="mqtt-port" value="1883">
            <div class="field-error" id="err-mqtt-port">Port is required</div>
          </div>
          <div class="form-group">
            <label class="label">Tenant ID <span class="req">*</span></label>
            <input type="text" id="mqtt-tenant" placeholder="e.g. dc1b368b-fde3-4559-..." maxlength="64">
            <div class="field-error" id="err-mqtt-tenant">Tenant ID is required</div>
          </div>
          <div class="form-group">
            <label class="label">Application ID <span class="req">*</span></label>
            <input type="text" id="mqtt-app" placeholder="e.g. sensornode-uat" maxlength="64">
            <div class="field-error" id="err-mqtt-app">Application ID is required</div>
          </div>
          <div class="form-group">
            <label class="label">Device ID (Client ID) <span class="req">*</span></label>
            <input type="text" id="mqtt-client" placeholder="e.g. d0ed1dac-8a2e-4744-..." maxlength="64">
            <div class="field-error" id="err-mqtt-client">Device ID is required</div>
          </div>
          <div class="form-group">
            <label class="label">Device Token (Username) <span class="req">*</span></label>
            <div class="input-wrap">
              <input type="password" id="mqtt-user" placeholder="Device token" maxlength="64">
              <button type="button" class="toggle-pw" onclick="togglePw('mqtt-user', this)">
                <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" class="eye-icon"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3"/></svg>
              </button>
            </div>
            <div class="field-error" id="err-mqtt-user">Device Token is required</div>
          </div>
          <div class="form-group">
            <label class="label">Device Password <span class="req">*</span></label>
            <div class="input-wrap">
              <input type="password" id="mqtt-pass" placeholder="Device password" maxlength="64">
              <button type="button" class="toggle-pw" onclick="togglePw('mqtt-pass', this)">
                <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" class="eye-icon"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3"/></svg>
              </button>
            </div>
            <div class="field-error" id="err-mqtt-pass">Device Password is required</div>
          </div>
        </div>
      </div>
      <div class="card-footer">
        <button class="btn btn-primary" id="btn-save-mqtt" onclick="saveMqtt()">Save & Continue</button>
        <button class="btn btn-ghost" onclick="goToPage('wifi')">Back</button>
        <button class="btn btn-outline" onclick="goToPage('ota')" style="margin-left:auto;">Skip</button>
      </div>
    </div>
  </div>

  <!-- OTA PAGE -->
  <div class="page" id="page-ota">
    <div class="page-title">
      <div class="step-label">Step 4 of 4</div>
      <h1>OTA Update</h1>
      <p>Over-the-air firmware update (optional)</p>
      <div class="accent-line"></div>
    </div>

    <div class="alert alert-warn">
      <svg width="13" height="13" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M10.29 3.86L1.82 18a2 2 0 0 0 1.71 3h16.94a2 2 0 0 0 1.71-3L13.71 3.86a2 2 0 0 0-3.42 0z"/><line x1="12" y1="9" x2="12" y2="13"/><line x1="12" y1="17" x2="12.01" y2="17"/></svg>
      Do not power off during update. The device reboots automatically when complete.
    </div>

    <div class="status-msg" id="ota-status"></div>

    <div class="card">
      <div class="card-header">
        <div class="card-title">Upload Firmware</div>
      </div>
      <div class="card-body">
        <div id="upload-area">
          <div class="upload-zone" id="drop-zone"
            onclick="document.getElementById('fw-file').click()"
            ondragover="event.preventDefault();this.classList.add('drag')"
            ondragleave="this.classList.remove('drag')"
            ondrop="handleDrop(event)">
            <div class="upload-icon-wrap">
              <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="16 16 12 12 8 16"/><line x1="12" y1="12" x2="12" y2="21"/><path d="M20.39 18.39A5 5 0 0 0 18 9h-1.26A8 8 0 1 0 3 16.3"/></svg>
            </div>
            <h3>Drop .bin file here</h3>
            <p>or click to browse</p>
            <input type="file" id="fw-file" accept=".bin" style="display:none" onchange="fileSelected(this)">
          </div>

          <div class="progress-wrap" id="upload-progress" style="display:none;">
            <div class="progress-meta">
              <span class="fname" id="upload-filename"></span>
              <span class="pct" id="upload-pct">0%</span>
            </div>
            <div class="progress-bar">
              <div class="progress-fill" id="progress-fill"></div>
            </div>
          </div>
        </div>

        <div class="success-banner" id="ota-success">
          <div class="success-icon">
            <svg width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><polyline points="20 6 9 17 4 12"/></svg>
          </div>
          <h3>Firmware uploaded successfully</h3>
          <p>Device is rebooting with the new firmware.</p>
        </div>
      </div>
      <div class="card-footer" id="ota-footer">
        <button class="btn btn-primary" id="upload-btn" onclick="uploadFirmware()" disabled>Upload Firmware</button>
        <button class="btn btn-ghost" onclick="goToPage('mqtt')">Back</button>
        <button class="btn btn-outline" onclick="goToPage('status')" style="margin-left:auto;">Done</button>
      </div>
    </div>
  </div>

</main>

<footer>
  <div class="footer-inner">
    <span>© 2025 Sensornode IoT Technology</span>
    <span>192.168.4.1</span>
  </div>
</footer>

<!-- Reboot Modal -->
<div class="reboot-modal" id="reboot-modal">
  <div class="reboot-modal-content">
    <h3>Reboot Device?</h3>
    <p>The device will restart and you may need to reconnect.</p>
    <div class="btn-row">
      <button class="btn btn-outline" onclick="hideRebootModal()">Cancel</button>
      <button class="btn btn-primary" onclick="doReboot()">Reboot Now</button>
    </div>
  </div>
</div>

<script>
const ORDER = ['status', 'wifi', 'mqtt', 'ota'];
const BAR = { status: '8%', wifi: '33%', mqtt: '66%', ota: '100%' };
let config = {};
let selectedFile = null;

// Load config on page load
window.onload = function() {
  loadConfig();
};

function loadConfig() {
  fetch('/api/config')
    .then(r => r.json())
    .then(data => {
      config = data;
      populateFields();
      updateStatusPage();
    })
    .catch(e => {
      console.error('Failed to load config:', e);
      document.getElementById('hero-title').textContent = 'Configuration not loaded';
      document.getElementById('hero-text').textContent = 'Unable to retrieve device settings.';
    });
}

function populateFields() {
  // WiFi
  if (config.wifi_ssid) document.getElementById('wifi-ssid').value = config.wifi_ssid;
  if (config.wifi_pass) document.getElementById('wifi-pass').value = config.wifi_pass;
  // MQTT
  if (config.mqtt_host) document.getElementById('mqtt-host').value = config.mqtt_host;
  if (config.mqtt_port) document.getElementById('mqtt-port').value = config.mqtt_port;
  if (config.tenant_id) document.getElementById('mqtt-tenant').value = config.tenant_id;
  if (config.application_id) document.getElementById('mqtt-app').value = config.application_id;
  if (config.mqtt_client_id) document.getElementById('mqtt-client').value = config.mqtt_client_id;
  if (config.mqtt_username) document.getElementById('mqtt-user').value = config.mqtt_username;
  if (config.mqtt_password) document.getElementById('mqtt-pass').value = config.mqtt_password;
}

function updateStatusPage() {
  const hasWifi = config.wifi_ssid && config.wifi_ssid.length > 0;
  const hasMqtt = config.mqtt_host && config.mqtt_host.length > 0;
  const hasDevice = config.mqtt_client_id && config.mqtt_username && config.mqtt_password;
  const isValid = hasWifi && hasMqtt && hasDevice;

  // Update display values
  setRow('cfg-ssid', config.wifi_ssid);
  setRow('cfg-host', config.mqtt_host);
  setRow('cfg-port', config.mqtt_port || '1883');
  setRow('cfg-tenant', config.tenant_id);
  setRow('cfg-app', config.application_id);
  setRow('cfg-device', config.mqtt_client_id);

  // Update checklist
  setCheck('chk-wifi-icon', hasWifi);
  setCheck('chk-mqtt-icon', hasMqtt);
  setCheck('chk-device-icon', hasDevice);

  // Update badge
  const badge = document.getElementById('cfg-badge');
  if (isValid) {
    badge.className = 'badge badge-green';
    badge.textContent = 'Ready';
  } else {
    badge.className = 'badge badge-red';
    badge.textContent = 'Incomplete';
  }

  // Update header dot
  const dot = document.getElementById('status-dot');
  const label = document.getElementById('status-label');
  if (isValid) {
    dot.className = 'dot online';
    label.textContent = 'ESP32 · Ready to connect';
  } else {
    dot.className = 'dot';
    label.textContent = 'ESP32 · Not provisioned';
  }

  // Update hero
  const heroTitle = document.getElementById('hero-title');
  const heroText = document.getElementById('hero-text');
  if (isValid) {
    heroTitle.textContent = 'Configuration complete';
    heroText.textContent = 'Reboot the device to connect to Sensorclouds.';
  } else {
    heroTitle.textContent = 'Configuration incomplete';
    heroText.textContent = 'Complete all steps to connect this device to Sensorclouds.';
  }
}

function setRow(id, val) {
  const el = document.getElementById(id);
  if (val && val.length > 0) {
    el.textContent = val;
    el.className = 'v';
  } else {
    el.textContent = 'Not configured';
    el.className = 'v empty';
  }
}

function setCheck(id, pass) {
  const el = document.getElementById(id);
  el.className = pass ? 'check-icon pass' : 'check-icon fail';
  el.innerHTML = pass
    ? '<svg width="10" height="10" viewBox="0 0 24 24" fill="none" stroke="#059669" stroke-width="3"><polyline points="20 6 9 17 4 12"/></svg>'
    : '<svg width="10" height="10" viewBox="0 0 24 24" fill="none" stroke="#EF4444" stroke-width="3"><line x1="18" y1="6" x2="6" y2="18"/><line x1="6" y1="6" x2="18" y2="18"/></svg>';
}

function goToPage(id) {
  ORDER.forEach(p => {
    document.getElementById('page-' + p).classList.toggle('active', p === id);
    document.getElementById('tab-' + p).classList.remove('active');
  });
  document.getElementById('tab-' + id).classList.add('active');
  document.getElementById('step-bar').style.width = BAR[id];
  window.scrollTo({ top: 0, behavior: 'smooth' });
}

function togglePw(inputId, btn) {
  const inp = document.getElementById(inputId);
  const showing = inp.type === 'text';
  inp.type = showing ? 'password' : 'text';
}

function showStatus(elemId, msg, isError) {
  const el = document.getElementById(elemId);
  el.textContent = msg;
  el.className = 'status-msg show ' + (isError ? 'error' : 'success');
  setTimeout(() => { el.className = 'status-msg'; }, 5000);
}

function validate(fields) {
  let ok = true;
  fields.forEach(([id, errId]) => {
    const inp = document.getElementById(id);
    const err = document.getElementById(errId);
    if (!inp.value.trim()) {
      inp.classList.add('invalid');
      err.classList.add('show');
      ok = false;
    } else {
      inp.classList.remove('invalid');
      err.classList.remove('show');
    }
  });
  return ok;
}

function saveWifi() {
  if (!validate([['wifi-ssid', 'err-wifi-ssid'], ['wifi-pass', 'err-wifi-pass']])) return;

  const btn = document.getElementById('btn-save-wifi');
  btn.disabled = true;
  btn.textContent = 'Saving...';

  const data = new URLSearchParams();
  data.append('ssid', document.getElementById('wifi-ssid').value);
  data.append('pass', document.getElementById('wifi-pass').value);

  fetch('/wifi', { method: 'POST', body: data })
    .then(r => r.ok ? r.text() : Promise.reject('Failed'))
    .then(() => {
      config.wifi_ssid = document.getElementById('wifi-ssid').value;
      config.wifi_pass = document.getElementById('wifi-pass').value;
      showStatus('wifi-status', 'WiFi settings saved successfully!', false);
      updateStatusPage();
      setTimeout(() => goToPage('mqtt'), 1000);
    })
    .catch(e => {
      showStatus('wifi-status', 'Failed to save WiFi settings. Please try again.', true);
    })
    .finally(() => {
      btn.disabled = false;
      btn.textContent = 'Save & Continue';
    });
}

function saveMqtt() {
  const fields = [
    ['mqtt-host', 'err-mqtt-host'],
    ['mqtt-port', 'err-mqtt-port'],
    ['mqtt-tenant', 'err-mqtt-tenant'],
    ['mqtt-app', 'err-mqtt-app'],
    ['mqtt-client', 'err-mqtt-client'],
    ['mqtt-user', 'err-mqtt-user'],
    ['mqtt-pass', 'err-mqtt-pass']
  ];
  if (!validate(fields)) return;

  const btn = document.getElementById('btn-save-mqtt');
  btn.disabled = true;
  btn.textContent = 'Saving...';

  const data = new URLSearchParams();
  data.append('host', document.getElementById('mqtt-host').value);
  data.append('port', document.getElementById('mqtt-port').value);
  data.append('tenant', document.getElementById('mqtt-tenant').value);
  data.append('app', document.getElementById('mqtt-app').value);
  data.append('client', document.getElementById('mqtt-client').value);
  data.append('user', document.getElementById('mqtt-user').value);
  data.append('pass', document.getElementById('mqtt-pass').value);

  fetch('/mqtt', { method: 'POST', body: data })
    .then(r => r.ok ? r.text() : Promise.reject('Failed'))
    .then(() => {
      config.mqtt_host = document.getElementById('mqtt-host').value;
      config.mqtt_port = document.getElementById('mqtt-port').value;
      config.tenant_id = document.getElementById('mqtt-tenant').value;
      config.application_id = document.getElementById('mqtt-app').value;
      config.mqtt_client_id = document.getElementById('mqtt-client').value;
      config.mqtt_username = document.getElementById('mqtt-user').value;
      config.mqtt_password = document.getElementById('mqtt-pass').value;
      showStatus('mqtt-status', 'MQTT settings saved successfully!', false);
      updateStatusPage();
      setTimeout(() => goToPage('ota'), 1000);
    })
    .catch(e => {
      showStatus('mqtt-status', 'Failed to save MQTT settings. Please try again.', true);
    })
    .finally(() => {
      btn.disabled = false;
      btn.textContent = 'Save & Continue';
    });
}

function showRebootModal() {
  document.getElementById('reboot-modal').classList.add('show');
}

function hideRebootModal() {
  document.getElementById('reboot-modal').classList.remove('show');
}

function doReboot() {
  fetch('/reboot', { method: 'POST' })
    .then(() => {
      document.getElementById('reboot-modal').querySelector('h3').textContent = 'Rebooting...';
      document.getElementById('reboot-modal').querySelector('p').textContent = 'Please wait and reconnect to your normal WiFi.';
      document.getElementById('reboot-modal').querySelector('.btn-row').style.display = 'none';
    });
}

// OTA
function fileSelected(input) {
  if (!input.files[0]) return;
  selectedFile = input.files[0];
  document.getElementById('upload-progress').style.display = 'block';
  document.getElementById('upload-filename').textContent = selectedFile.name;
  document.getElementById('progress-fill').style.width = '0%';
  document.getElementById('upload-pct').textContent = '0%';
  document.getElementById('upload-btn').disabled = false;
}

function handleDrop(e) {
  e.preventDefault();
  document.getElementById('drop-zone').classList.remove('drag');
  const f = e.dataTransfer.files[0];
  if (f && f.name.endsWith('.bin')) {
    selectedFile = f;
    document.getElementById('upload-progress').style.display = 'block';
    document.getElementById('upload-filename').textContent = f.name;
    document.getElementById('progress-fill').style.width = '0%';
    document.getElementById('upload-pct').textContent = '0%';
    document.getElementById('upload-btn').disabled = false;
  } else {
    showStatus('ota-status', 'Please drop a .bin firmware file.', true);
  }
}

function uploadFirmware() {
  if (!selectedFile) return;

  const btn = document.getElementById('upload-btn');
  btn.disabled = true;
  btn.textContent = 'Uploading...';

  const formData = new FormData();
  formData.append('firmware', selectedFile);

  const xhr = new XMLHttpRequest();
  xhr.open('POST', '/update', true);

  xhr.upload.onprogress = function(e) {
    if (e.lengthComputable) {
      const pct = Math.round((e.loaded / e.total) * 100);
      document.getElementById('progress-fill').style.width = pct + '%';
      document.getElementById('upload-pct').textContent = pct + '%';
    }
  };

  xhr.onload = function() {
    if (xhr.status === 200) {
      document.getElementById('upload-area').style.display = 'none';
      document.getElementById('ota-success').classList.add('show');
      document.getElementById('ota-footer').innerHTML =
        '<button class="btn btn-primary" onclick="goToPage(\'status\')">Back to Status</button>';
    } else {
      showStatus('ota-status', 'Firmware upload failed. Please try again.', true);
      btn.disabled = false;
      btn.textContent = 'Upload Firmware';
    }
  };

  xhr.onerror = function() {
    showStatus('ota-status', 'Network error during upload.', true);
    btn.disabled = false;
    btn.textContent = 'Upload Firmware';
  };

  xhr.send(formData);
}

// Clear validation on input
document.querySelectorAll('input').forEach(inp => {
  inp.addEventListener('input', () => {
    inp.classList.remove('invalid');
    const errId = 'err-' + inp.id;
    const err = document.getElementById(errId);
    if (err) err.classList.remove('show');
  });
});
</script>
</body>
</html>
)rawliteral";
