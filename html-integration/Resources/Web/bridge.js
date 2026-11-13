/* Native audio bridge. No simulated audio, no network dependencies. */
(() => {
  'use strict';
  const backend = window.__JUCE__?.backend;
  const knobs = Array.from(document.querySelectorAll('.knob-base[data-param]'));
  const byParam = new Map(knobs.map(k => [k.dataset.param, k]));
  let state = null, dragging = null, view = 'scope', source = 'out';
  let queued = null, frame = null;
  const emit = (event, payload) => backend?.emitEvent(event, payload);
  const active = id => !!state?.parameters?.[id]?.active;
  const send = (id, value, phase = 'value') => emit('bmParam', {id: 'bm_' + id, value, phase});
  const clamp = (n, lo, hi) => Math.max(lo, Math.min(hi, n));
  const snap = (knob,value) => knob.dataset.step ? Math.round(value/Number(knob.dataset.step))*Number(knob.dataset.step) : value;
  function updateKnob(knob, value, updateLabel = true) {
    const min = Number(knob.dataset.min), max = Number(knob.dataset.max);
    knob.dataset.val = String(value);
    const cap = knob.querySelector('.knob-cap');
    if (cap) cap.style.transform = `rotate(${-135 + 270 * clamp((value-min)/(max-min), 0, 1)}deg)`;
    if (!updateLabel) return;
    const label = document.getElementById('val-' + knob.dataset.param);
    if (!label) return;
    const unit = knob.dataset.unit || '', param = knob.dataset.param;
    label.textContent = param === 'xl_mode' ? ['Densité','Présence','Impact'][Math.round(value)]
      : param==='thd' && value<=-60 ? 'OFF'
      : param === 'pan' ? (Math.abs(value) < .01 ? 'C' : `${value<0?'L':'R'}${Math.round(Math.abs(value))}`)
      : unit === 'dB' ? `${value>0?'+':''}${value.toFixed(1)} dB`
      : unit === '%' ? `${Math.round(value)}%`
      : unit === ':1' ? `${value.toFixed(1)}:1`
      : unit === 'Hz' || unit === 'ms' ? `${Math.round(value)} ${unit}`
      : `${value.toFixed(1)}${unit ? ' ' + unit : ''}`;
    knob.setAttribute('aria-valuenow', String(value));
    if(param==='xl_mode')knob.setAttribute('aria-valuetext',label.textContent);
  }
  function finishDrag() {
    if (!dragging) return;
    if (frame !== null) cancelAnimationFrame(frame);
    frame = null;
    if (queued !== null) send(dragging.id, queued);
    send(dragging.id, Number(dragging.knob.dataset.val), 'end');
    if(dragging.knob.hasPointerCapture?.(dragging.pointerId)) dragging.knob.releasePointerCapture(dragging.pointerId);
    dragging = null; queued = null; document.body.style.cursor = '';
  }
  knobs.forEach(knob => {
    const id = knob.dataset.param;
    knob.setAttribute('role', 'slider'); knob.tabIndex = 0;
    knob.setAttribute('aria-label', id);
    knob.setAttribute('aria-valuemin', knob.dataset.min); knob.setAttribute('aria-valuemax', knob.dataset.max);
    updateKnob(knob, Number(knob.dataset.val), false);
    knob.addEventListener('pointerdown', e => {
      if (!active(id) || e.button !== 0) return;
      e.preventDefault(); knob.setPointerCapture(e.pointerId);
      dragging = {id, knob, startY:e.clientY, startVal:Number(knob.dataset.val), pointerId:e.pointerId};
      send(id, dragging.startVal, 'begin'); document.body.style.cursor = 'ns-resize';
    });
    knob.addEventListener('pointermove', e => {
      if (dragging?.knob !== knob) return;
      const range = Number(knob.dataset.max)-Number(knob.dataset.min);
      queued = snap(knob,clamp(dragging.startVal + (dragging.startY-e.clientY)*range/(e.shiftKey?1500:150), Number(knob.dataset.min),Number(knob.dataset.max)));
      updateKnob(knob, queued);
      if (frame === null) frame = requestAnimationFrame(() => {frame=null;if(dragging && queued!==null){send(id,queued);queued=null;}});
    });
    knob.addEventListener('pointerup', finishDrag); knob.addEventListener('pointercancel', finishDrag);
    knob.addEventListener('dblclick', () => {
      if (!active(id)) return;
      const v = state.parameters[id].defaultValue; send(id,v,'begin');send(id,v);send(id,v,'end');updateKnob(knob,v);
    });
    knob.addEventListener('keydown', e => {
      if (!active(id) || !['ArrowUp','ArrowDown','ArrowLeft','ArrowRight'].includes(e.key)) return;
      e.preventDefault(); const direction=['ArrowUp','ArrowRight'].includes(e.key)?1:-1;
      const step=Number(knob.dataset.step)||(Number(knob.dataset.max)-Number(knob.dataset.min))/(e.shiftKey?1000:100);
      const v=clamp(Number(knob.dataset.val)+direction*step,Number(knob.dataset.min),Number(knob.dataset.max));
      send(id,v,'begin');send(id,v);send(id,v,'end');updateKnob(knob,v);
    });
  });
  window.addEventListener('blur',finishDrag);
  const toggleMap = {bypassBtn:'bypass',modeToggle:'valve',ispBtn:'isp',eqActiveBtn:'eq_active',deSoloBtn:'spike_listen',mudActiveBtn:'mud_active',ringActiveBtn:'ring_active',thdColorBtn:'thd_mode',tapeModeBtn:'tape_mode'};
  for (const [element,id] of Object.entries(toggleMap)) document.getElementById(element)?.addEventListener('click', () => {
    if(!active(id)) return;
    const v=state.parameters[id].value>.5?0:1;send(id,v,'begin');send(id,v);send(id,v,'end');
  });
  document.querySelectorAll('.profile-btn').forEach(btn=>btn.addEventListener('click',()=>{
    if(!active('profile')) return;
    const v=Number(btn.dataset.prof)-1;send('profile',v,'begin');send('profile',v);send('profile',v,'end');
  }));
  const selector=document.getElementById('presetSelector');
  selector?.addEventListener('change',()=>{if(selector.value!=='')emit('bmAction',{action:'preset',index:Number(selector.value)});});
  document.getElementById('abBtn')?.addEventListener('click',()=>emit('bmAction',{action:'ab'}));
  const target=document.getElementById('lufsTargetSelector');
  if(target){target.disabled=true;target.title='Repère visuel uniquement : normalisation LUFS non implémentée.';}
  for(const [id,mode] of [['viewScopeBtn','scope'],['viewEqBtn','eq'],['viewMetersBtn','meters']])
    document.getElementById(id)?.addEventListener('click',()=>{
      view=mode;
      for(const [b,m] of [['viewScopeBtn','scope'],['viewEqBtn','eq'],['viewMetersBtn','meters']])
        document.getElementById(b).className=m===view?'px-2 py-0.5 rounded bg-cyan-950 border border-cyan-800 text-cyan-300 font-bold transition':'px-2 py-0.5 rounded text-neutral-400 hover:text-white transition';
      draw();
    });
  document.getElementById('meterSourceBtn')?.addEventListener('click',()=>{source=source==='out'?'in':'out';document.getElementById('meterSourceBtn').textContent=source.toUpperCase();draw();});
  document.getElementById('monitorMatchBtn')?.addEventListener('click',()=>emit('bmAction',{action:'monitorMatch'}));
  document.getElementById('resetMetersBtn')?.addEventListener('click',()=>emit('bmAction',{action:'resetMeters'}));
  const loudnessText = measurement => {
    const lu = value => Number.isFinite(value) && value > -199 ? value.toFixed(1) : '—';
    const tp = measurement?.truePeak > 0 ? (20*Math.log10(measurement.truePeak)).toFixed(1) : '—';
    return `I ${lu(measurement?.integrated)} LUFS / S ${lu(measurement?.shortTerm)} LUFS / TP ${tp} dBTP`;
  };
  function apply(next) {
    if(!next.scope && state?.scope)next.scope=state.scope;
    state=next;
    const monitor=document.getElementById('monitorMatchBtn');
    if(monitor){
      monitor.textContent=next.monitorMatch?(next.monitorReady?`Comp. écoute ${(next.monitorGainDb||0).toFixed(1)} dB`:'Comp. écoute : apprentissage'):'Comp. écoute OFF';
      monitor.classList.toggle('btn-active',!!next.monitorMatch);monitor.setAttribute('aria-pressed',String(!!next.monitorMatch));
      monitor.title='Écoute temporaire : atténue le plus fort (maximum 12 dB). Mesures avant compensation. COUPER avant export temps réel ; désactivée lors du rendu hors ligne signalé par l’hôte.';
    }
    const readout=document.getElementById('loudnessReadout');if(readout)readout.textContent=loudnessText(next.loudness);
    if(!selector.dataset.native){
      selector.replaceChildren(new Option('Réglages personnalisés',''),...next.presets.map((name,i)=>new Option(name,String(i))));
      selector.dataset.native='true';
    }
    selector.value=next.preset>=0?String(next.preset):'';
    for(const [id,p] of Object.entries(next.parameters)) {
      const knob=byParam.get(id);
      if(knob){
        knob.setAttribute('aria-disabled',String(!p.active));
        knob.style.cursor=p.active?'ns-resize':'not-allowed';
        knob.title=p.active?(id==='lpf'?`Fréquence effective : ${next.lpfEffective.toFixed(0)} Hz (selon Fs)`:'Glisser verticalement ; Maj = réglage fin ; double-clic = défaut'):'Fonction non raccordée au DSP dans cette version';
        if(id==='thd')knob.title='Dosage de coloration : OFF à −60 dB ; cette échelle ne mesure pas le THD de sortie';
        if(id==='tape_color')knob.title='Coloration de bande indépendante de la compression Tape Glue ; 0 % = désactivée';
        if(id==='glue')knob.title='Compression de bus ; le dosage Tape Color règle séparément la coloration';
        if(dragging?.id!==id && p.active)updateKnob(knob,p.value);
      }
    }
    for(const [element,id] of Object.entries(toggleMap)){
      const button=document.getElementById(element),p=next.parameters[id];if(!button||!p)continue;
      button.disabled=!p.active;button.title=p.active?'':'Fonction non raccordée au DSP';
      button.classList.toggle(element==='modeToggle'?'toggle-active':'btn-active',p.value>.5);
      button.setAttribute('aria-pressed',String(p.value>.5));
      if(element==='deSoloBtn')button.title='Écouter le signal retiré par De-Harsh, Mud Trap et Ring Trap';
      if(element==='modeToggle')button.title='Valve : récupération optical du compresseur et légère coloration à lampes';
      if(element==='thdColorBtn'){
        button.textContent=p.value>.5?'THD : Dense':'THD : Warm';
        button.title=p.value>.5?'Symétrique : harmoniques impaires':'Asymétrique : harmoniques paires';
      }
      if(element==='tapeModeBtn'){
        button.textContent=p.value>.5?'Marqué':'Discret';
        button.title='Caractère Tape : saturation douce ou plus dense, sans modifier la compression Glue';
      }
      if(element==='mudActiveBtn'||element==='ringActiveBtn'){
        button.textContent=p.value>.5?'ON':'OFF';
        button.setAttribute('aria-label',`${element==='mudActiveBtn'?'Mud Trap':'Ring Trap'} : ${p.value>.5?'actif':'désactivé'}`);
      }
    }
    for (let band=1;band<=3;band++) {
      const f=next.parameters[`p${band}f`]?.value,g=next.parameters[`p${band}g`]?.value;
      const container=byParam.get(`p${band}f`)?.parentElement?.parentElement;
      if(container && Number.isFinite(f) && Number.isFinite(g)) {
        const frequency=f>=1000?`${(f/1000).toFixed(1).replace(/\.0$/,'')}kHz`:`${Math.round(f)}Hz`;
        container.lastElementChild.textContent=`${frequency} / ${g>0?'+':''}${g.toFixed(1)}dB`;
      }
    }
    const eqLabel=document.querySelector('#eqActiveBtn span:last-child');
    if(eqLabel)eqLabel.textContent=next.parameters.eq_active.value>.5?'EQ ON':'EQ OFF';
    const valve=next.parameters.valve.value>.5;
    document.getElementById('modeValveLbl').className=valve?'text-red-400 font-bold':'text-neutral-500';
    document.getElementById('modePureLbl').className=valve?'text-neutral-500':'text-sky-400 font-bold';
    document.querySelectorAll('.profile-btn').forEach(btn=>{
      const index=Number(btn.dataset.prof)-1;
      btn.disabled=!next.parameters.profile.active;
      btn.title=['Ouvert : shelf 2,5 kHz','Présence : cloche 3,5 kHz','Air : shelf 7 kHz','Chaleur : cloche 1,2 kHz'][index];
      btn.classList.toggle('btn-active',index===next.parameters.profile.value);
      btn.setAttribute('aria-pressed',String(index===next.parameters.profile.value));
    });
    const ab=document.getElementById('abBtn');ab.title=`Comparaison ${next.ab} active — ${next.monitorMatch?'compensation d’écoute activée':'compensation d’écoute désactivée'}`;ab.setAttribute('aria-label',ab.title);
    ab.classList.toggle('btn-active',next.ab==='B');
    for(const span of document.querySelectorAll('span')){
      if(span.textContent.startsWith('SAMPLE RATE:'))span.textContent=`SAMPLE RATE: ${(next.sampleRate/1000).toFixed(1)} kHz / CALCUL DSP 64-BIT`;
      else if(span.textContent.startsWith('LATENCY:'))span.textContent=`LATENCY: ${next.latency} SAMPLES`;
      else if(span.textContent==='● VST3 / AU / AAX')span.textContent='● VST3 WINDOWS';
      else if(span.textContent==='bloody Master v1.0.0')span.textContent='bloody Master v0.3.5';
    }
    draw();
  }
  const canvas=document.getElementById('precisionMeterCanvas'),ctx=canvas.getContext('2d');
  function draw(){
    const rect=canvas.getBoundingClientRect(),dpr=window.devicePixelRatio||1,W=rect.width,H=rect.height;
    if(W<=0||H<=0)return;
    if(canvas.width!==Math.round(W*dpr)||canvas.height!==Math.round(H*dpr)){canvas.width=Math.round(W*dpr);canvas.height=Math.round(H*dpr);}
    ctx.setTransform(dpr,0,0,dpr,0,0);ctx.fillStyle='#041619';ctx.fillRect(0,0,W,H);
    ctx.strokeStyle='rgba(18,85,96,.45)';ctx.lineWidth=1;ctx.setLineDash([2,4]);
    for(let i=1;i<10;i++){ctx.beginPath();ctx.moveTo(W*i/10,0);ctx.lineTo(W*i/10,H);ctx.stroke();}
    for(let i=1;i<6;i++){ctx.beginPath();ctx.moveTo(0,H*i/6);ctx.lineTo(W,H*i/6);ctx.stroke();}
    ctx.setLineDash([]);ctx.fillStyle='#22d3ee';ctx.font='9px monospace';
    if(!state){ctx.fillText('CONNEXION AU MOTEUR AUDIO…',12,20);return;}
    const meters=state.meters,db=v=>20*Math.log10(Math.max(v,1e-6));
    const peaks=source==='out'?[meters.outputL,meters.outputR]:[meters.inputL,meters.inputR];
    if(view==='scope'){
      ctx.fillText(`OSCILLOSCOPE ${source.toUpperCase()} — AUDIO RÉEL`,12,16);
      const traces=state.scope?.[source]||[[],[]];
      traces.forEach((trace,c)=>{ctx.strokeStyle=c===0?'#22d3ee':'#fbbf24';ctx.shadowBlur=5;ctx.shadowColor=ctx.strokeStyle;ctx.beginPath();
        if(!trace.length){ctx.moveTo(0,H/2);ctx.lineTo(W,H/2);}else trace.forEach((v,i)=>{const x=i*W/Math.max(1,trace.length-1),y=H/2-clamp(v,-1,1)*H*.38;i?ctx.lineTo(x,y):ctx.moveTo(x,y);});ctx.stroke();ctx.shadowBlur=0;});
      ctx.fillStyle='#22d3ee';ctx.fillText(`L ${db(peaks[0]).toFixed(1)} / R ${db(peaks[1]).toFixed(1)} dBFS`,12,H-12);
    }else if(view==='eq'){
      ctx.fillText('COURBE EQ CALCULÉE — FILTRES DU DSP',12,16);
      ctx.strokeStyle='#22d3ee';ctx.lineWidth=1.5;ctx.beginPath();
      (state.eq||[]).forEach(([hz,gain],i)=>{const x=12+(W-24)*Math.log(hz/20)/Math.log(1000),y=H/2-clamp(gain,-18,18)*H/48;i?ctx.lineTo(x,y):ctx.moveTo(x,y);});ctx.stroke();
      ctx.fillStyle='#94a3b8';ctx.fillText('20 Hz',10,H-10);ctx.fillText('20 kHz',W-50,H-10);
    }else{
      ctx.fillText(`CRÊTES ${source.toUpperCase()} (dBFS) / RÉDUCTION (dB)`,10,16);
      const bars=[{label:'L',value:(db(peaks[0])+60)/60,text:db(peaks[0]).toFixed(1),color:'#22d3ee'},
        {label:'R',value:(db(peaks[1])+60)/60,text:db(peaks[1]).toFixed(1),color:'#22d3ee'},
        {label:'COMP',value:meters.compressorGr/12,text:meters.compressorGr.toFixed(1),color:'#fbbf24'},
        {label:'LIM',value:meters.limiterGr/12,text:meters.limiterGr.toFixed(1),color:'#ef4444'}];
      bars.forEach((bar,i)=>{const x=18+i*(W-36)/4,bottom=H-40,height=H-78;ctx.fillStyle='#0f3035';ctx.fillRect(x,30,20,height);ctx.fillStyle=bar.color;ctx.fillRect(x,bottom-height*clamp(bar.value,0,1),20,height*clamp(bar.value,0,1));ctx.fillText(bar.label,x-2,H-25);ctx.fillText(bar.text,x-5,H-12);});
      ctx.fillStyle='#94a3b8';ctx.fillText(loudnessText(state.loudness),12,H-1);
    }
  }
  window.addEventListener('resize',draw);
  backend?.addEventListener('bmState',apply);
  window.bloodyMasterUi={applyState:apply,draw,getState:()=>state};
  emit('bmReady',{});draw();
})();
