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
    --blue: #0047FF;
    --blue-mid: #25A0FF;
    --blue-light: #E9EBFF;
    --bg: #F0F4FF;
    --white: #FFFFFF;
    --dark: #1A1A2E;
    --mid: #4A5068;
    --light: #8A93B2;
    --border: #DDE3F5;
    --red: #EF4444;
    --green: #10B981;
    --orange: #F59E0B;
    --radius: 10px;
    --radius-sm: 6px;
    --shadow: 0 1px 4px rgba(0,71,255,0.08);
  }
  *, *::before, *::after { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    font-family: 'Segoe UI', -apple-system, BlinkMacSystemFont, sans-serif;
    background: var(--bg);
    color: var(--dark);
    min-height: 100vh;
    font-size: 14px;
    display: flex;
    flex-direction: column;
  }

  /* Header */
  header { background: var(--white); border-bottom: 1px solid var(--border); position: sticky; top: 0; z-index: 100; }
  .header-inner { max-width: 680px; margin: 0 auto; padding: 0 20px; }
  .brand-row { display: flex; align-items: center; justify-content: space-between; padding: 13px 0 11px; border-bottom: 1px solid var(--border); }
  .sn-logo { display: flex; align-items: center; gap: 9px; text-decoration: none; }
  .sn-wordmark { display: flex; flex-direction: column; line-height: 1; }
  .sn-wordmark .name { font-size: 13px; font-weight: 700; color: var(--blue); letter-spacing: -0.2px; font-family: 'Courier New', monospace; }
  .sn-wordmark .sub { font-size: 8.5px; letter-spacing: 1.4px; color: var(--light); text-transform: uppercase; margin-top: 2px; }
  .device-badge { display: flex; align-items: center; gap: 6px; font-size: 11px; color: var(--light); }
  .dot { width: 6px; height: 6px; border-radius: 50%; background: var(--red); flex-shrink: 0; }
  .dot.online { background: var(--green); }

  /* Nav */
  .nav-tabs { display: flex; }
  .tab { flex: 1; display: flex; align-items: center; justify-content: center; gap: 5px; padding: 10px 6px; font-size: 12px; font-weight: 500; color: var(--light); cursor: pointer; border-bottom: 2px solid transparent; transition: color 0.15s; user-select: none; }
  .tab:hover { color: var(--blue); }
  .tab.active { color: var(--blue); font-weight: 600; }
  .tab.done { color: var(--green); }
  .tab.done .step-num { background: var(--green); }
  .step-num { width: 16px; height: 16px; border-radius: 50%; background: var(--border); color: var(--white); font-size: 9px; font-weight: 700; display: flex; align-items: center; justify-content: center; flex-shrink: 0; }
  .tab.active .step-num { background: var(--blue); }
  .step-progress { height: 3px; background: var(--border); }
  .step-progress-fill { height: 100%; background: linear-gradient(90deg, var(--blue), var(--blue-mid)); transition: width 0.35s ease; width: 0%; }

  /* Main */
  main { max-width: 680px; margin: 0 auto; padding: 28px 20px 56px; flex: 1; width: 100%; }
  .page { display: none; }
  .page.active { display: block; }
  .page-title { margin-bottom: 20px; }
  .page-title h1 { font-size: 19px; font-weight: 700; letter-spacing: -0.3px; }
  .page-title p { font-size: 12px; color: var(--light); margin-top: 3px; }
  .accent-line { width: 32px; height: 3px; background: var(--blue); border-radius: 2px; margin-top: 8px; }
  .step-label { display: inline-flex; align-items: center; gap: 6px; font-size: 11px; font-weight: 600; color: var(--blue); background: var(--blue-light); border-radius: 20px; padding: 3px 10px; margin-bottom: 10px; }

  /* Hero */
  .hero { background: linear-gradient(135deg, var(--blue) 0%, #1469FF 55%, var(--blue-mid) 100%); border-radius: var(--radius); padding: 18px 22px; color: #fff; margin-bottom: 14px; position: relative; overflow: hidden; }
  .hero.ready { background: linear-gradient(135deg, var(--green) 0%, #059669 100%); }
  .hero::after { content: ''; position: absolute; right: -18px; top: -18px; width: 110px; height: 110px; border-radius: 50%; background: rgba(255,255,255,0.06); }
  .hero h2 { font-size: 15px; font-weight: 700; position: relative; display: flex; align-items: center; gap: 8px; }
  .hero p { font-size: 12px; opacity: .85; margin-top: 4px; position: relative; }
  .hero .btn { margin-top: 12px; background: rgba(255,255,255,0.2); border: 1px solid rgba(255,255,255,0.3); }
  .hero .btn:hover { background: rgba(255,255,255,0.3); }

  /* Cards */
  .card { background: var(--white); border: 1px solid var(--border); border-radius: var(--radius); box-shadow: var(--shadow); }
  .card + .card { margin-top: 14px; }
  .card-header { padding: 14px 20px; border-bottom: 1px solid var(--border); display: flex; align-items: center; justify-content: space-between; }
  .card-title { font-size: 13px; font-weight: 600; color: var(--dark); display: flex; align-items: center; gap: 7px; }
  .card-title svg { color: var(--blue); }
  .card-body { padding: 20px; }
  .card-footer { padding: 14px 20px; border-top: 1px solid var(--border); display: flex; gap: 8px; align-items: center; }

  /* Status - Checklist Items */
  .setup-item { display: flex; align-items: center; gap: 14px; padding: 14px 20px; border-bottom: 1px solid var(--border); cursor: pointer; transition: background 0.15s; }
  .setup-item:last-child { border-bottom: none; }
  .setup-item:hover { background: var(--bg); }
  .setup-icon { width: 36px; height: 36px; border-radius: 8px; display: flex; align-items: center; justify-content: center; flex-shrink: 0; }
  .setup-icon.pending { background: #FEF3C7; color: var(--orange); }
  .setup-icon.done { background: #D1FAE5; color: var(--green); }
  .setup-icon.fail { background: #FEE2E2; color: var(--red); }
  .setup-content { flex: 1; min-width: 0; }
  .setup-content h4 { font-size: 13px; font-weight: 600; color: var(--dark); margin-bottom: 2px; }
  .setup-content p { font-size: 11px; color: var(--light); white-space: nowrap; overflow: hidden; text-overflow: ellipsis; }
  .setup-content p.configured { color: var(--green); font-weight: 500; }
  .setup-arrow { color: var(--light); flex-shrink: 0; }

  /* Info rows */
  .info-row { display: flex; justify-content: space-between; align-items: center; padding: 10px 20px; border-bottom: 1px solid var(--border); font-size: 12px; gap: 12px; }
  .info-row:last-child { border-bottom: none; }
  .info-row .k { color: var(--light); white-space: nowrap; }
  .info-row .v { font-weight: 500; text-align: right; word-break: break-all; max-width: 200px; overflow: hidden; text-overflow: ellipsis; }
  .info-row .v.empty { color: var(--light); font-style: italic; font-weight: 400; }

  /* Badge */
  .badge { display: inline-flex; align-items: center; gap: 4px; padding: 3px 8px; border-radius: 20px; font-size: 11px; font-weight: 600; }
  .badge-red { background: #FEE2E2; color: var(--red); }
  .badge-green { background: #D1FAE5; color: #059669; }
  .badge-orange { background: #FEF3C7; color: #D97706; }

  /* Reboot Card */
  .reboot-card { background: linear-gradient(135deg, #FEF3C7 0%, #FDE68A 100%); border: 1px solid #FCD34D; }
  .reboot-card .card-body { display: flex; align-items: center; gap: 16px; padding: 16px 20px; }
  .reboot-icon { width: 44px; height: 44px; background: var(--white); border-radius: 10px; display: flex; align-items: center; justify-content: center; color: var(--orange); flex-shrink: 0; box-shadow: 0 2px 8px rgba(0,0,0,0.08); }
  .reboot-content { flex: 1; }
  .reboot-content h4 { font-size: 14px; font-weight: 600; color: #92400E; margin-bottom: 2px; }
  .reboot-content p { font-size: 12px; color: #A16207; }
  .btn-reboot { background: #D97706; color: white; padding: 10px 20px; white-space: nowrap; }
  .btn-reboot:hover { background: #B45309; }

  /* Forms */
  .grid { display: grid; grid-template-columns: repeat(auto-fit, minmax(220px, 1fr)); gap: 14px; }
  .form-group { margin-bottom: 16px; }
  .form-group:last-child { margin-bottom: 0; }
  .label { display: block; font-size: 12px; font-weight: 600; color: var(--mid); margin-bottom: 5px; }
  .label .req { color: var(--red); margin-left: 2px; }
  .hint { font-size: 11px; color: var(--light); margin-top: 4px; }
  .input-wrap { position: relative; }
  .input-wrap input { padding-right: 38px; }
  .toggle-pw { position: absolute; right: 10px; top: 50%; transform: translateY(-50%); background: none; border: none; cursor: pointer; color: var(--light); padding: 2px; display: flex; }
  .toggle-pw:hover { color: var(--blue); }
  input[type=text], input[type=password], input[type=number] { width: 100%; padding: 9px 12px; border: 1px solid var(--border); border-radius: var(--radius-sm); font-size: 13px; color: var(--dark); background: var(--white); font-family: inherit; outline: none; transition: border-color 0.15s, box-shadow 0.15s; }
  input:focus { border-color: var(--blue); box-shadow: 0 0 0 3px rgba(0,71,255,0.10); }
  input::placeholder { color: var(--light); }
  input.invalid { border-color: var(--red); }
  .field-error { font-size: 11px; color: var(--red); margin-top: 4px; display: none; }
  .field-error.show { display: block; }

  /* Buttons */
  .btn { display: inline-flex; align-items: center; gap: 6px; padding: 9px 18px; border-radius: var(--radius-sm); font-size: 13px; font-weight: 600; cursor: pointer; border: none; font-family: inherit; transition: all 0.15s; }
  .btn-primary { background: var(--blue); color: #fff; }
  .btn-primary:hover { background: #0038D4; box-shadow: 0 4px 12px rgba(0,71,255,0.28); transform: translateY(-1px); }
  .btn-outline { background: transparent; color: var(--mid); border: 1px solid var(--border); }
  .btn-outline:hover { border-color: var(--blue); color: var(--blue); }
  .btn-ghost { background: transparent; color: var(--light); border: none; padding: 9px 12px; }
  .btn-ghost:hover { color: var(--blue); }
  .btn:disabled { opacity: .4; cursor: not-allowed; transform: none !important; box-shadow: none !important; }

  /* Alerts */
  .alert { display: flex; align-items: flex-start; gap: 9px; padding: 11px 14px; border-radius: var(--radius-sm); font-size: 12px; line-height: 1.5; margin-bottom: 18px; }
  .alert svg { flex-shrink: 0; margin-top: 1px; }
  .alert-info { background: var(--blue-light); border: 1px solid #C7D2FE; color: #3730A3; }
  .alert-warn { background: #FFFBEB; border: 1px solid #FDE68A; color: #92400E; }
  .status-msg { padding: 10px 14px; border-radius: var(--radius-sm); font-size: 12px; margin-bottom: 14px; display: none; }
  .status-msg.show { display: block; }
  .status-msg.success { background: #D1FAE5; color: #065F46; border: 1px solid #6EE7B7; }
  .status-msg.error { background: #FEE2E2; color: #991B1B; border: 1px solid #FECACA; }

  /* OTA */
  .upload-zone { border: 2px dashed var(--border); border-radius: var(--radius); padding: 36px 20px; text-align: center; cursor: pointer; transition: border-color 0.2s, background 0.2s; background: rgba(0,71,255,0.02); }
  .upload-zone:hover, .upload-zone.drag { border-color: var(--blue); background: rgba(0,71,255,0.04); }
  .upload-icon-wrap { width: 46px; height: 46px; background: var(--blue-light); border-radius: 50%; display: flex; align-items: center; justify-content: center; margin: 0 auto 12px; color: var(--blue); }
  .upload-zone h3 { font-size: 13px; font-weight: 600; }
  .upload-zone p { font-size: 12px; color: var(--light); margin-top: 3px; }
  .progress-wrap { margin-top: 14px; }
  .progress-meta { display: flex; justify-content: space-between; font-size: 12px; margin-bottom: 5px; }
  .progress-meta .fname { color: var(--mid); font-weight: 500; overflow: hidden; text-overflow: ellipsis; white-space: nowrap; max-width: 76%; }
  .progress-meta .pct { color: var(--blue); font-weight: 700; }
  .progress-bar { background: var(--blue-light); border-radius: 20px; height: 5px; overflow: hidden; }
  .progress-fill { height: 100%; background: linear-gradient(90deg, var(--blue), var(--blue-mid)); border-radius: 20px; width: 0%; transition: width 0.3s ease; }
  .success-banner { display: none; flex-direction: column; align-items: center; text-align: center; padding: 28px 20px; gap: 10px; }
  .success-banner.show { display: flex; }
  .success-icon { width: 52px; height: 52px; background: #D1FAE5; border-radius: 50%; display: flex; align-items: center; justify-content: center; color: #059669; }
  .success-banner h3 { font-size: 15px; font-weight: 700; color: var(--dark); }
  .success-banner p { font-size: 12px; color: var(--light); }

  /* Modal */
  .modal { display: none; position: fixed; top: 0; left: 0; right: 0; bottom: 0; background: rgba(0,0,0,0.5); z-index: 200; align-items: center; justify-content: center; padding: 20px; }
  .modal.show { display: flex; }
  .modal-content { background: var(--white); padding: 24px; border-radius: var(--radius); max-width: 340px; width: 100%; text-align: center; }
  .modal-icon { width: 56px; height: 56px; border-radius: 50%; display: flex; align-items: center; justify-content: center; margin: 0 auto 16px; }
  .modal-icon.warn { background: #FEF3C7; color: var(--orange); }
  .modal-icon.loading { background: var(--blue-light); color: var(--blue); }
  .modal h3 { font-size: 16px; margin-bottom: 8px; }
  .modal p { font-size: 13px; color: var(--mid); margin-bottom: 20px; line-height: 1.5; }
  .modal .btn-row { display: flex; gap: 10px; justify-content: center; }
  .spinner { animation: spin 1s linear infinite; }
  @keyframes spin { to { transform: rotate(360deg); } }

  /* Footer */
  footer { border-top: 1px solid var(--border); background: var(--white); }
  .footer-inner { max-width: 680px; margin: 0 auto; padding: 12px 20px; display: flex; align-items: center; justify-content: space-between; font-size: 11px; color: var(--light); }
</style>
</head>
<body>

<header>
  <div class="header-inner">
    <div class="brand-row" style="justify-content:center;border-bottom:none;padding:14px 0;">
      <span style="font-size:16px;font-weight:700;color:var(--dark);">Device Configuration | Connecting with Sensorclouds</span>
    </div>
    <nav class="nav-tabs">
      <div class="tab active" id="tab-status" onclick="goToPage('status')"><span class="step-num">1</span>Status</div>
      <div class="tab" id="tab-wifi" onclick="goToPage('wifi')"><span class="step-num">2</span>WiFi</div>
      <div class="tab" id="tab-mqtt" onclick="goToPage('mqtt')"><span class="step-num">3</span>MQTT</div>
      <div class="tab" id="tab-ota" onclick="goToPage('ota')"><span class="step-num">4</span>OTA</div>
    </nav>
    <div class="step-progress"><div class="step-progress-fill" id="step-bar"></div></div>
  </div>
</header>

<main>
  <!-- STATUS PAGE -->
  <div class="page active" id="page-status">
    <div class="page-title">
      <div class="step-label">Step 1 of 4</div>
      <h1>Device Status</h1>
      <p>Review configuration and complete setup steps</p>
      <div class="accent-line"></div>
    </div>

    <div class="hero" id="hero-banner">
      <h2 id="hero-title">
        <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="12"/><line x1="12" y1="16" x2="12.01" y2="16"/></svg>
        <span id="hero-title-text">Loading...</span>
      </h2>
      <p id="hero-text">Please wait while we load the device settings.</p>
      <button class="btn" id="hero-btn" onclick="goToPage('wifi')" style="display:none;">
        <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M5 12h14M12 5l7 7-7 7"/></svg>
        Start Configuration
      </button>
    </div>

    <!-- Setup Checklist as clickable items -->
    <div class="card">
      <div class="card-header">
        <div class="card-title">
          <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="9 11 12 14 22 4"/><path d="M21 12v7a2 2 0 0 1-2 2H5a2 2 0 0 1-2-2V5a2 2 0 0 1 2-2h11"/></svg>
          Setup Progress
        </div>
        <span class="badge badge-orange" id="progress-badge">0 of 3</span>
      </div>

      <div class="setup-item" onclick="goToPage('wifi')" id="setup-wifi">
        <div class="setup-icon pending" id="setup-wifi-icon">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M5 12.55a11 11 0 0 1 14.08 0"/><path d="M1.42 9a16 16 0 0 1 21.16 0"/><path d="M8.53 16.11a6 6 0 0 1 6.95 0"/><line x1="12" y1="20" x2="12.01" y2="20"/></svg>
        </div>
        <div class="setup-content">
          <h4>WiFi Network</h4>
          <p id="setup-wifi-status">Not configured — tap to set up</p>
        </div>
        <div class="setup-arrow">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="9 18 15 12 9 6"/></svg>
        </div>
      </div>

      <div class="setup-item" onclick="goToPage('mqtt')" id="setup-mqtt">
        <div class="setup-icon pending" id="setup-mqtt-icon">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="2" y="2" width="20" height="8" rx="2"/><rect x="2" y="14" width="20" height="8" rx="2"/><line x1="6" y1="6" x2="6.01" y2="6"/><line x1="6" y1="18" x2="6.01" y2="18"/></svg>
        </div>
        <div class="setup-content">
          <h4>MQTT Broker</h4>
          <p id="setup-mqtt-status">Not configured — tap to set up</p>
        </div>
        <div class="setup-arrow">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="9 18 15 12 9 6"/></svg>
        </div>
      </div>

      <div class="setup-item" onclick="goToPage('mqtt')" id="setup-device">
        <div class="setup-icon pending" id="setup-device-icon">
          <svg width="18" height="18" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><rect x="4" y="4" width="16" height="16" rx="2"/><rect x="9" y="9" width="6" height="6"/><line x1="9" y1="1" x2="9" y2="4"/><line x1="15" y1="1" x2="15" y2="4"/><line x1="9" y1="20" x2="9" y2="23"/><line x1="15" y1="20" x2="15" y2="23"/><line x1="20" y1="9" x2="23" y2="9"/><line x1="20" y1="14" x2="23" y2="14"/><line x1="1" y1="9" x2="4" y2="9"/><line x1="1" y1="14" x2="4" y2="14"/></svg>
        </div>
        <div class="setup-content">
          <h4>Device Credentials</h4>
          <p id="setup-device-status">Not configured — tap to set up</p>
        </div>
        <div class="setup-arrow">
          <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="9 18 15 12 9 6"/></svg>
        </div>
      </div>
    </div>

    <!-- Configuration Summary (collapsible) -->
    <details class="card" style="margin-top:14px;">
      <summary class="card-header" style="cursor:pointer;list-style:none;">
        <div class="card-title">
          <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><circle cx="12" cy="12" r="3"/><path d="M19.4 15a1.65 1.65 0 0 0 .33 1.82l.06.06a2 2 0 0 1 0 2.83 2 2 0 0 1-2.83 0l-.06-.06a1.65 1.65 0 0 0-1.82-.33 1.65 1.65 0 0 0-1 1.51V21a2 2 0 0 1-2 2 2 2 0 0 1-2-2v-.09A1.65 1.65 0 0 0 9 19.4a1.65 1.65 0 0 0-1.82.33l-.06.06a2 2 0 0 1-2.83 0 2 2 0 0 1 0-2.83l.06-.06a1.65 1.65 0 0 0 .33-1.82 1.65 1.65 0 0 0-1.51-1H3a2 2 0 0 1-2-2 2 2 0 0 1 2-2h.09A1.65 1.65 0 0 0 4.6 9a1.65 1.65 0 0 0-.33-1.82l-.06-.06a2 2 0 0 1 0-2.83 2 2 0 0 1 2.83 0l.06.06a1.65 1.65 0 0 0 1.82.33H9a1.65 1.65 0 0 0 1-1.51V3a2 2 0 0 1 2-2 2 2 0 0 1 2 2v.09a1.65 1.65 0 0 0 1 1.51 1.65 1.65 0 0 0 1.82-.33l.06-.06a2 2 0 0 1 2.83 0 2 2 0 0 1 0 2.83l-.06.06a1.65 1.65 0 0 0-.33 1.82V9a1.65 1.65 0 0 0 1.51 1H21a2 2 0 0 1 2 2 2 2 0 0 1-2 2h-.09a1.65 1.65 0 0 0-1.51 1z"/></svg>
          Configuration Details
        </div>
        <svg width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="var(--light)" stroke-width="2"><polyline points="6 9 12 15 18 9"/></svg>
      </summary>
      <div class="info-row"><span class="k">WiFi SSID</span><span class="v empty" id="cfg-ssid">Not set</span></div>
      <div class="info-row"><span class="k">MQTT Host</span><span class="v empty" id="cfg-host">Not set</span></div>
      <div class="info-row"><span class="k">MQTT Port</span><span class="v" id="cfg-port">1883</span></div>
      <div class="info-row"><span class="k">Tenant ID</span><span class="v empty" id="cfg-tenant">Not set</span></div>
      <div class="info-row"><span class="k">App ID</span><span class="v empty" id="cfg-app">Not set</span></div>
      <div class="info-row"><span class="k">Device ID</span><span class="v empty" id="cfg-device">Not set</span></div>
    </details>

    <!-- Reboot Card -->
    <div class="card reboot-card" style="margin-top:14px;">
      <div class="card-body">
        <div class="reboot-icon">
          <svg width="22" height="22" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="23 4 23 10 17 10"/><path d="M20.49 15a9 9 0 1 1-2.12-9.36L23 10"/></svg>
        </div>
        <div class="reboot-content">
          <h4>Restart Device</h4>
          <p>Apply settings and reconnect to your network</p>
        </div>
        <button class="btn btn-reboot" onclick="showRebootModal()">
          <svg width="14" height="14" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2.5"><polyline points="23 4 23 10 17 10"/><path d="M20.49 15a9 9 0 1 1-2.12-9.36L23 10"/></svg>
          Reboot
        </button>
      </div>
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
      <div class="card-header"><div class="card-title">Network Credentials</div></div>
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
              <button type="button" class="toggle-pw" onclick="togglePw('wifi-pass')">
                <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3"/></svg>
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
      <div class="card-header"><div class="card-title">Broker & Identity</div></div>
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
              <button type="button" class="toggle-pw" onclick="togglePw('mqtt-user')">
                <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3"/></svg>
              </button>
            </div>
            <div class="field-error" id="err-mqtt-user">Device Token is required</div>
          </div>
          <div class="form-group">
            <label class="label">Device Password <span class="req">*</span></label>
            <div class="input-wrap">
              <input type="password" id="mqtt-pass" placeholder="Device password" maxlength="64">
              <button type="button" class="toggle-pw" onclick="togglePw('mqtt-pass')">
                <svg width="15" height="15" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M1 12s4-8 11-8 11 8 11 8-4 8-11 8-11-8-11-8z"/><circle cx="12" cy="12" r="3"/></svg>
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
      <div class="card-header"><div class="card-title">Upload Firmware</div></div>
      <div class="card-body">
        <div id="upload-area">
          <div class="upload-zone" id="drop-zone" onclick="document.getElementById('fw-file').click()" ondragover="event.preventDefault();this.classList.add('drag')" ondragleave="this.classList.remove('drag')" ondrop="handleDrop(event)">
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
            <div class="progress-bar"><div class="progress-fill" id="progress-fill"></div></div>
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
    <span> Sensorclouds by Sensornode © 2026 - All rights reserved</span>
  </div>
</footer>

<!-- Reboot Modal -->
<div class="modal" id="reboot-modal">
  <div class="modal-content" id="reboot-modal-content">
    <div class="modal-icon warn" id="reboot-icon">
      <svg width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><polyline points="23 4 23 10 17 10"/><path d="M20.49 15a9 9 0 1 1-2.12-9.36L23 10"/></svg>
    </div>
    <h3 id="reboot-title">Restart Device?</h3>
    <p id="reboot-text">The device will restart and attempt to connect using your saved settings. You'll need to reconnect to your regular WiFi network.</p>
    <div class="btn-row" id="reboot-buttons">
      <button class="btn btn-outline" onclick="hideRebootModal()">Cancel</button>
      <button class="btn btn-reboot" onclick="doReboot()">Restart Now</button>
    </div>
  </div>
</div>

<script>
const ORDER = ['status', 'wifi', 'mqtt', 'ota'];
const BAR = { status: '8%', wifi: '33%', mqtt: '66%', ota: '100%' };
let config = {};
let selectedFile = null;

window.onload = function() { loadConfig(); };

function loadConfig() {
  fetch('/api/config')
    .then(r => r.json())
    .then(data => { config = data; populateFields(); updateStatusPage(); })
    .catch(e => {
      document.getElementById('hero-title-text').textContent = 'Connection Error';
      document.getElementById('hero-text').textContent = 'Unable to load device configuration.';
    });
}

function populateFields() {
  if (config.wifi_ssid) document.getElementById('wifi-ssid').value = config.wifi_ssid;
  if (config.wifi_pass) document.getElementById('wifi-pass').value = config.wifi_pass;
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
  const doneCount = (hasWifi ? 1 : 0) + (hasMqtt ? 1 : 0) + (hasDevice ? 1 : 0);

  // Update config details
  setRow('cfg-ssid', config.wifi_ssid);
  setRow('cfg-host', config.mqtt_host);
  setRow('cfg-port', config.mqtt_port || '1883');
  setRow('cfg-tenant', config.tenant_id);
  setRow('cfg-app', config.application_id);
  setRow('cfg-device', config.mqtt_client_id);

  // Update setup items
  setSetupItem('wifi', hasWifi, config.wifi_ssid ? 'Connected to ' + config.wifi_ssid : 'Not configured — tap to set up');
  setSetupItem('mqtt', hasMqtt, config.mqtt_host ? config.mqtt_host + ':' + (config.mqtt_port || 1883) : 'Not configured — tap to set up');
  setSetupItem('device', hasDevice, hasDevice ? 'ID: ' + config.mqtt_client_id.substring(0, 12) + '...' : 'Not configured — tap to set up');

  // Update progress badge
  const badge = document.getElementById('progress-badge');
  badge.textContent = doneCount + ' of 3';
  badge.className = 'badge ' + (isValid ? 'badge-green' : 'badge-orange');

  // Update header
  const dot = document.getElementById('status-dot');
  const label = document.getElementById('status-label');
  dot.className = isValid ? 'dot online' : 'dot';
  label.textContent = isValid ? 'ESP32 · Ready' : 'ESP32 · Setup required';

  // Update hero
  const hero = document.getElementById('hero-banner');
  const heroTitle = document.getElementById('hero-title-text');
  const heroText = document.getElementById('hero-text');
  const heroBtn = document.getElementById('hero-btn');
  const heroIcon = document.getElementById('hero-title').querySelector('svg');

  if (isValid) {
    hero.className = 'hero ready';
    heroTitle.textContent = 'Ready to Connect';
    heroText.textContent = 'All settings configured. Restart the device to apply changes and connect to Sensorclouds.';
    heroIcon.innerHTML = '<polyline points="20 6 9 17 4 12"/>';
    heroBtn.style.display = 'none';
  } else {
    hero.className = 'hero';
    heroTitle.textContent = 'Setup Required';
    heroText.textContent = 'Complete the steps below to configure your device for Sensorclouds.';
    heroIcon.innerHTML = '<circle cx="12" cy="12" r="10"/><line x1="12" y1="8" x2="12" y2="12"/><line x1="12" y1="16" x2="12.01" y2="16"/>';
    heroBtn.style.display = 'inline-flex';
    heroBtn.onclick = function() { goToPage(hasWifi ? (hasMqtt ? 'mqtt' : 'mqtt') : 'wifi'); };
  }
}

function setRow(id, val) {
  const el = document.getElementById(id);
  if (val && val.length > 0) { el.textContent = val; el.className = 'v'; }
  else { el.textContent = 'Not set'; el.className = 'v empty'; }
}

function setSetupItem(name, done, statusText) {
  const icon = document.getElementById('setup-' + name + '-icon');
  const status = document.getElementById('setup-' + name + '-status');
  icon.className = 'setup-icon ' + (done ? 'done' : 'pending');
  status.textContent = statusText;
  status.className = done ? 'configured' : '';
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

function togglePw(inputId) {
  const inp = document.getElementById(inputId);
  inp.type = inp.type === 'password' ? 'text' : 'password';
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
    if (!inp.value.trim()) { inp.classList.add('invalid'); err.classList.add('show'); ok = false; }
    else { inp.classList.remove('invalid'); err.classList.remove('show'); }
  });
  return ok;
}

function saveWifi() {
  if (!validate([['wifi-ssid', 'err-wifi-ssid'], ['wifi-pass', 'err-wifi-pass']])) return;
  const btn = document.getElementById('btn-save-wifi');
  btn.disabled = true; btn.textContent = 'Saving...';
  const data = new URLSearchParams();
  data.append('ssid', document.getElementById('wifi-ssid').value);
  data.append('pass', document.getElementById('wifi-pass').value);
  fetch('/wifi', { method: 'POST', body: data })
    .then(r => r.ok ? r.text() : Promise.reject('Failed'))
    .then(() => {
      config.wifi_ssid = document.getElementById('wifi-ssid').value;
      config.wifi_pass = document.getElementById('wifi-pass').value;
      showStatus('wifi-status', 'WiFi settings saved!', false);
      updateStatusPage();
      setTimeout(() => goToPage('mqtt'), 800);
    })
    .catch(e => showStatus('wifi-status', 'Failed to save. Please try again.', true))
    .finally(() => { btn.disabled = false; btn.textContent = 'Save & Continue'; });
}

function saveMqtt() {
  const fields = [['mqtt-host', 'err-mqtt-host'], ['mqtt-port', 'err-mqtt-port'], ['mqtt-tenant', 'err-mqtt-tenant'], ['mqtt-app', 'err-mqtt-app'], ['mqtt-client', 'err-mqtt-client'], ['mqtt-user', 'err-mqtt-user'], ['mqtt-pass', 'err-mqtt-pass']];
  if (!validate(fields)) return;
  const btn = document.getElementById('btn-save-mqtt');
  btn.disabled = true; btn.textContent = 'Saving...';
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
      showStatus('mqtt-status', 'MQTT settings saved!', false);
      updateStatusPage();
      setTimeout(() => goToPage('ota'), 800);
    })
    .catch(e => showStatus('mqtt-status', 'Failed to save. Please try again.', true))
    .finally(() => { btn.disabled = false; btn.textContent = 'Save & Continue'; });
}

function showRebootModal() { document.getElementById('reboot-modal').classList.add('show'); }
function hideRebootModal() { document.getElementById('reboot-modal').classList.remove('show'); }

function doReboot() {
  const icon = document.getElementById('reboot-icon');
  const title = document.getElementById('reboot-title');
  const text = document.getElementById('reboot-text');
  const buttons = document.getElementById('reboot-buttons');
  icon.className = 'modal-icon loading';
  icon.innerHTML = '<svg class="spinner" width="28" height="28" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2"><path d="M21 12a9 9 0 1 1-6.219-8.56"/></svg>';
  title.textContent = 'Restarting...';
  text.textContent = 'Please wait. Reconnect to your regular WiFi network after the device restarts.';
  buttons.style.display = 'none';
  fetch('/reboot', { method: 'POST' });
}

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
  } else { showStatus('ota-status', 'Please drop a .bin file.', true); }
}

function uploadFirmware() {
  if (!selectedFile) return;
  const btn = document.getElementById('upload-btn');
  btn.disabled = true; btn.textContent = 'Uploading...';
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
      document.getElementById('ota-footer').innerHTML = '<button class="btn btn-primary" onclick="goToPage(\'status\')">Back to Status</button>';
    } else {
      showStatus('ota-status', 'Upload failed. Please try again.', true);
      btn.disabled = false; btn.textContent = 'Upload Firmware';
    }
  };
  xhr.onerror = function() {
    showStatus('ota-status', 'Network error.', true);
    btn.disabled = false; btn.textContent = 'Upload Firmware';
  };
  xhr.send(formData);
}

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
