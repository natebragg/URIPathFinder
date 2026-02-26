// URIPathFinder: A simple parser for URIs
//
// BSD 3-Clause License
//
// Copyright (c) 2024, Nate Bragg
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef URI_PATH_FINDER_SCHEME
#define URI_PATH_FINDER_SCHEME

typedef enum scheme {
    ERROR = -1,
    aaa = 0, // Diameter Protocol: RFC6733
    aaas, // Diameter Protocol with Secure Transport: RFC6733
    about, // RFC6694
    acap, // application configuration access protocol: RFC2244
    acct, // RFC7565
    acd,
    acr,
    adiumxtra,
    adt,
    afp,
    afs, // Andrew File System global file names: RFC1738
    aim,
    amss,
    android,
    appdata,
    apt,
    ar,
    ark,
    at,
    attachment,
    aw,
    barion,
    bb,
    beshare,
    bitcoin,
    bitcoincash,
    blob,
    bluetooth,
    bolo,
    brid,
    browserext,
    cabal,
    calculator,
    callto,
    cap, // Calendar Access Protocol: RFC4324
    cast,
    casts,
    chrome,
    chrome_extension, // chrome-extension
    cid, // content identifier: RFC2392
    coap, // RFC7252
    coap_tcp, // coap+tcp: RFC8323
    coap_ws, // coap+ws: RFC8323
    coaps, // RFC7252
    coaps_tcp, // coaps+tcp: RFC8323
    coaps_ws, // coaps+ws: RFC8323
    com_eventbrite_attendee, // com-eventbrite-attendee
    content,
    content_type, // content-type
    crid, // TV-Anytime Content Reference Identifier: RFC4078
    cstr,
    cvs,
    dab,
    dat,
    data, // RFC2397
    dav, // RFC4918
    dhttp,
    diaspora,
    dict, // dictionary service protocol: RFC2229
    did,
    dis,
    dlna_playcontainer, // dlna-playcontainer
    dlna_playsingle, // dlna-playsingle
    dns, // Domain Name System: RFC4501
    dntp,
    doi,
    dpp,
    drm,
    drop,
    dtmi,
    dtn, // DTNRG research and development: RFC9171
    dvb,
    dvx,
    dweb,
    ed2k,
    eid,
    elsi,
    embedded,
    ens,
    ethereum,
    example, // RFC7595
    facetime,
    fax, // RFC2806 RFC3966
    feed,
    feedready,
    fido,
    file, // Host-specific file names: RFC8089
    filesystem,
    finger,
    first_run_pen_experience, // first-run-pen-experience
    fish,
    fm,
    ftp, // File Transfer Protocol: RFC1738
    fuchsia_pkg, // fuchsia-pkg
    geo, // Geographic Locations: RFC5870
    gg,
    git,
    gitoid,
    gizmoproject,
    go, // RFC3368
    gopher, // The Gopher Protocol: RFC4266
    graph,
    grd,
    gtalk,
    h323, // RFC3508
    ham, // RFC7046
    hcap,
    hcp,
    hs20,
    http, // Hypertext Transfer Protocol: RFC8615, RFC9110, Section 4.2.1
    https, // Hypertext Transfer Protocol Secure: RFC8615, RFC9110, Section 4.2.2
    hxxp,
    hxxps,
    hydrazone,
    hyper,
    iax, // Inter-Asterisk eXchange Version 2: RFC5456
    icap, // Internet Content Adaptation Protocol: RFC3507
    icon,
    im, // Instant Messaging: RFC3860
    imap, // internet message access protocol: RFC5092
    info, // Information Assets with Identifiers in Public Namespaces: RFC4452
    iotdisco,
    ipfs,
    ipn, // RFC-ietf-dtn-ipn-update-14
    ipns,
    ipp, // Internet Printing Protocol: RFC3510
    ipps, // Internet Printing Protocol over HTTPS: RFC7472
    irc,
    irc6,
    ircs,
    iris, // Internet Registry Information Service: RFC3981
    iris_beep, // iris.beep: RFC3983
    iris_lwz, // iris.lwz: RFC4993
    iris_xpc, // iris.xpc: RFC4992
    iris_xpcs, // iris.xpcs: RFC4992
    isostore,
    itms,
    jabber,
    jar,
    jms, // Java Message Service: RFC6167
    keyparc,
    lastfm,
    lbry,
    ldap, // Lightweight Directory Access Protocol: RFC4516
    ldaps,
    leaptofrogans, // RFC8589
    lid,
    lorawan,
    lpa,
    lvlt,
    machineProvisioningProgressReporter, // Windows Autopilot Modern Device Management status updates
    magnet,
    mailserver, // Access to data available from mail servers: RFC6196
    mailto, // Electronic mail address: RFC6068
    maps,
    market,
    matrix,
    message,
    microsoft_windows_camera, // microsoft.windows.camera
    microsoft_windows_camera_multipicker, // microsoft.windows.camera.multipicker
    microsoft_windows_camera_picker, // microsoft.windows.camera.picker
    mid, // message identifier: RFC2392
    mms,
    modem, // RFC2806, RFC3966
    mongodb,
    moz,
    ms_access, // ms-access
    ms_appinstaller, // ms-appinstaller
    ms_browser_extension, // ms-browser-extension
    ms_calculator, // ms-calculator
    ms_drive_to, // ms-drive-to
    ms_enrollment, // ms-enrollment
    ms_excel, // ms-excel
    ms_eyecontrolspeech, // ms-eyecontrolspeech
    ms_gamebarservices, // ms-gamebarservices
    ms_gamingoverlay, // ms-gamingoverlay
    ms_getoffice, // ms-getoffice
    ms_help, // ms-help
    ms_infopath, // ms-infopath
    ms_inputapp, // ms-inputapp
    ms_launchremotedesktop, // ms-launchremotedesktop
    ms_lockscreencomponent_config, // ms-lockscreencomponent-config
    ms_media_stream_id, // ms-media-stream-id
    ms_meetnow, // ms-meetnow
    ms_mixedrealitycapture, // ms-mixedrealitycapture
    ms_mobileplans, // ms-mobileplans
    ms_newsandinterests, // ms-newsandinterests
    ms_officeapp, // ms-officeapp
    ms_people, // ms-people
    ms_personacard, // ms-personacard
    ms_powerpoint, // ms-powerpoint
    ms_project, // ms-project
    ms_publisher, // ms-publisher
    ms_recall, // ms-recall
    ms_remotedesktop, // ms-remotedesktop
    ms_remotedesktop_launch, // ms-remotedesktop-launch
    ms_restoretabcompanion, // ms-restoretabcompanion
    ms_screenclip, // ms-screenclip
    ms_screensketch, // ms-screensketch
    ms_search, // ms-search
    ms_search_repair, // ms-search-repair
    ms_secondary_screen_controller, // ms-secondary-screen-controller
    ms_secondary_screen_setup, // ms-secondary-screen-setup
    ms_settings, // ms-settings
    ms_settings_airplanemode, // ms-settings-airplanemode
    ms_settings_bluetooth, // ms-settings-bluetooth
    ms_settings_camera, // ms-settings-camera
    ms_settings_cellular, // ms-settings-cellular
    ms_settings_cloudstorage, // ms-settings-cloudstorage
    ms_settings_connectabledevices, // ms-settings-connectabledevices
    ms_settings_displays_topology, // ms-settings-displays-topology
    ms_settings_emailandaccounts, // ms-settings-emailandaccounts
    ms_settings_language, // ms-settings-language
    ms_settings_location, // ms-settings-location
    ms_settings_lock, // ms-settings-lock
    ms_settings_nfctransactions, // ms-settings-nfctransactions
    ms_settings_notifications, // ms-settings-notifications
    ms_settings_power, // ms-settings-power
    ms_settings_privacy, // ms-settings-privacy
    ms_settings_proximity, // ms-settings-proximity
    ms_settings_screenrotation, // ms-settings-screenrotation
    ms_settings_wifi, // ms-settings-wifi
    ms_settings_workplace, // ms-settings-workplace
    ms_spd, // ms-spd
    ms_stickers, // ms-stickers
    ms_sttoverlay, // ms-sttoverlay
    ms_transit_to, // ms-transit-to
    ms_useractivityset, // ms-useractivityset
    ms_virtualtouchpad, // ms-virtualtouchpad
    ms_visio, // ms-visio
    ms_walk_to, // ms-walk-to
    ms_whiteboard, // ms-whiteboard
    ms_whiteboard_cmd, // ms-whiteboard-cmd
    ms_word, // ms-word
    msnim,
    msrp, // Message Session Relay Protocol: RFC4975
    msrps, // Message Session Relay Protocol Secure: RFC4975, RFC8873
    mss,
    mt, // Matter protocol on-boarding payloads that are encoded for use in QR Codes and/or NFC Tags
    mtqp, // Message Tracking Query Protocol: RFC3887
    mumble,
    mupdate, // Mailbox Update (MUPDATE) Protocol: RFC3656
    mvn,
    mvrp,
    mvrps,
    news, // USENET news: RFC5538
    nfs, // network file system protocol: RFC2224
    ni, // RFC6920
    nih, // RFC6920
    nntp, // USENET news using NNTP access: RFC5538
    notes,
    num, // Namespace Utility Modules
    ocf,
    oid,
    onenote,
    onenote_cmd, // onenote-cmd
    opaquelocktoken, // RFC4918
    openid, // OpenID Connect
    openpgp4fpr,
    otpauth,
    p1,
    pack,
    palm,
    paparazzi,
    payment,
    payto, // RFC8905
    pkcs11, // PKCS#11: RFC7512
    platform,
    pop, // Post Office Protocol v3: RFC2384
    pres, // Presence: RFC3859
    prospero, // Prospero Directory Service: RFC4157
    proxy,
    psyc,
    pttp,
    pwid,
    qb,
    query,
    quic_transport, // quic-transport
    redis,
    rediss,
    reload, // RFC6940
    res,
    resource,
    rmi,
    rsync, // RFC5781
    rtmfp, // RFC7425
    rtmp,
    rtsp, // Real-Time Streaming Protocol (RTSP): RFC2326, RFC7826
    rtsps, // Real-Time Streaming Protocol (RTSP) over TLS: RFC2326, RFC7826
    rtspu, // Real-Time Streaming Protocol (RTSP) over unreliable datagram transport: RFC2326
    sarif,
    secondlife,
    secret_token, // secret-token: RFC8959
    service, // RFC2609
    session, // RFC6787
    sftp,
    sgn,
    shc,
    shelter,
    sieve, // ManageSieve Protocol: RFC5804
    simpleledger,
    simplex,
    sip, // session initiation protocol: RFC3261
    sips, // secure session initiation protocol: RFC3261
    skype,
    smb,
    smp,
    sms, // Short Message Service: RFC5724
    smtp,
    snews, // NNTP over SSL/TLS: RFC5538
    snmp, // Simple Network Management Protocol: RFC4088
    soap_beep, // soap.beep: RFC4227
    soap_beeps, // soap.beeps: RFC4227
    soldat,
    spiffe,
    spotify,
    ssb,
    ssh,
    starknet,
    steam,
    stun, // RFC7064
    stuns, // RFC7064
    submit,
    svn,
    swh,
    swid, // RFC9393, Section 5.1
    swidpath, // RFC9393, Section 5.2
    tag, // RFC4151
    taler,
    teamspeak,
    teapot,
    teapots,
    tel, // telephone: RFC3966, RFC5341
    teliaeid,
    telnet, // Reference to interactive sessions: RFC4248
    tftp, // Trivial File Transfer Protocol: RFC3617
    things,
    thismessage, // multipart/related relative reference resolution: RFC2557
    thzp,
    tip, // Transaction Internet Protocol: RFC2371
    tn3270, // Interactive 3270 emulation sessions: RFC6270
    tool,
    turn, // RFC7065
    turns, // RFC7065
    tv, // TV Broadcasts: RFC2838
    udp,
    unreal,
    upt,
    urn, // Uniform Resource Names: RFC8141
    ut2004,
    uuid_in_package, // uuid-in-package
    v_event, // v-event
    vemmi, // versatile multimedia interface: RFC2122
    ventrilo,
    ves,
    videotex, // RFC2122, RFC3986
    view_source, // view-source
    vnc, // Remote Framebuffer Protocol: RFC7869
    vscode,
    vscode_insiders, // vscode-insiders
    vsls,
    w3,
    wais, // Wide Area Information Servers: RFC4156
    wcr,
    web3,
    web_ap, // web+ap
    webcal,
    wifi,
    wpid,
    ws, // WebSocket connections: RFC8307, RFC6455
    wss, // Encrypted WebSocket connections: RFC8307, RFC6455
    wtai,
    wyciwyg,
    xcon, // RFC6501
    xcon_userid, // xcon_userid: RFC6501: xcon-userid
    xfire,
    xftp,
    xmlrpc_beep, // xmlrpc.beep: RFC3529
    xmlrpc_beeps, // xmlrpc.beeps: RFC3529
    xmpp, // Extensible Messaging and Presence Protocol: RFC5122
    xrcp,
    xri,
    ymsgr,
    z39_50, // z39.50: Z39.50 information access: RFC1738, RFC2056
    z39_50r, // z39.50r: Z39.50 Retrieval: RFC2056
    z39_50s, // z39.50s: Z39.50 Session: RFC2056
} scheme;

#endif
