import { NgModule } from '@angular/core';
import { CommonModule } from '@angular/common';

import { EvaluacionesRoutingModule } from './evaluaciones-routing.module';
import { EvaluacionesComponent } from './evaluaciones/evaluaciones.component';
import { CrearEvaluacionComponent } from './crear-evaluacion/crear-evaluacion.component';
import { ListadoComponent } from './listado/listado.component';
import { EvaluarComponent } from './evaluar/evaluar.component';
import { RankingYEdicionComponent } from './ranking-y-edicion/ranking-y-edicion.component';
import { ComparacionComponent } from './comparacion/comparacion.component';
import { RankingGlobalComponent } from './ranking-global/ranking-global.component';
import { DetalleComponent } from './detalle/detalle.component';
import { SharedModule } from '../utilidades/shared/shared.module';
import { FormsModule } from '@angular/forms';
import { MenuComponent } from './menu/menu.component';
import { ConceptoComponent } from './concepto/concepto.component';
import { CrearCompComponent } from './crear-comp/crear-comp.component';
import { MisEvaluacionesComponent } from './mis-evaluaciones/mis-evaluaciones.component';


@NgModule({
  declarations: [
    EvaluacionesComponent,
    CrearEvaluacionComponent,
    ListadoComponent,
    EvaluarComponent,
    RankingYEdicionComponent,
    ComparacionComponent,
    RankingGlobalComponent,
    DetalleComponent,
    MenuComponent,
    ConceptoComponent,
    CrearCompComponent,
    MisEvaluacionesComponent
  ],
  imports: [
    CommonModule,
    EvaluacionesRoutingModule,
    SharedModule,
    FormsModule
  ],
  entryComponents: [CrearCompComponent]
})
export class EvaluacionesModule { }
