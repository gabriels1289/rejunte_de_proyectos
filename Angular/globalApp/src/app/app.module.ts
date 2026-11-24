import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';
import { FormsModule } from '@angular/forms';
import { RouterModule, Routes, Router } from '@angular/router';
import { HttpClientModule, HTTP_INTERCEPTORS } from '@angular/common/http';

import { AppRoutingModule } from './app-routing.module';
import { AppComponent } from './app.component';

import { EmpleadosModule } from './empleados/empleados.module';
import { MensajeriaModule } from './mensajeria/mensajeria.module';
import { AuthModule } from './auth/auth.module';

// para uso http
import { HttpErrorHandlerService } from './_helpers/http-error-handler.service';
import { JwtInterceptor } from './auth/jwt.interceptor';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import { DialogoConfirmacionComponent } from './modales/dialogo-confirmacion/dialogo-confirmacion.component';
import { PopupInformativoComponent } from './modales/popup-informativo/popup-informativo.component';
import { MatDialogModule, MatButtonModule, MAT_DATE_LOCALE } from '@angular/material';
// para cambiar el idioma a español argentina
import { LOCALE_ID } from '@angular/core';
import { registerLocaleData } from '@angular/common';
import localeEsAr from '@angular/common/locales/es-AR';
import { SeguimientosModule } from './seguimientos/seguimientos.module';
import { EvaluacionesModule } from './evaluaciones/evaluaciones.module';
import { RecursosModule } from './recursos/recursos.module';
import { MisAjustesModule } from './mis-ajustes/mis-ajustes.module';
registerLocaleData(localeEsAr, 'es-AR');

@NgModule({
  declarations: [
    AppComponent,
    DialogoConfirmacionComponent,
    PopupInformativoComponent
    // PaginadorPipe,
  ],
  entryComponents: [
    DialogoConfirmacionComponent,
    PopupInformativoComponent
  ],
  imports: [
    BrowserModule,
    // import HttpClientModule after BrowserModule
    HttpClientModule,
    FormsModule,
    EmpleadosModule,
    MensajeriaModule,
    SeguimientosModule,
    EvaluacionesModule,
    RecursosModule,
    MisAjustesModule,
    AuthModule,
    AppRoutingModule,
    BrowserAnimationsModule,
    MatDialogModule,
    MatButtonModule
  ],
  exports: [
  ],
  providers: [
    HttpErrorHandlerService,
    { provide: HTTP_INTERCEPTORS, useClass: JwtInterceptor, multi: true },
    { provide: LOCALE_ID, useValue: 'es-Ar' },
    {provide: MAT_DATE_LOCALE, useValue: 'es-AR'},
  ],
  bootstrap: [AppComponent]
})
export class AppModule {
  // Diagnostic only: inspect router configuration
  constructor(router: Router) {
    // Use a custom replacer to display function names in the route configs
    const replacer = (key, value) => (typeof value === 'function') ? value.name : value;

    // console.log('Routes: ', JSON.stringify(router.config, replacer, 2));
  }
}
