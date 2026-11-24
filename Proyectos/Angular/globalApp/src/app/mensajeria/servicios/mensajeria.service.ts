import { Injectable } from '@angular/core';
import { HttpClient, HttpHeaders, HttpParams, HttpResponse, HttpRequest, HttpEventType } from '@angular/common/http';
import { Observable, Subject } from 'rxjs';
import { catchError } from 'rxjs/operators';

import { environment } from '../../../environments/environment';
import { Correo } from '../comunes/correo';

import { HttpErrorHandlerService, handleError } from '../../_helpers/http-error-handler.service';

const httpOptions = {
  headers: new HttpHeaders({
  //  'Content-Type': 'application/json',
    // 'Authorization': 'my-auth-token' // TODO: agregar el authService y obtener el token y el id del usuario
  })
};

@Injectable({
  providedIn: 'root'
})
export class MensajeriaService {
  // correos: Correo[];
  private handleError: handleError;

  constructor(
    private http: HttpClient,
    private httpErrorhandler: HttpErrorHandlerService
    ) {
    this.handleError = httpErrorhandler.createHandleError('MensajeriaService');
  }

  getBandejaDeEntrada(idusuario: number): Observable<any[] | HttpResponse<Correo[]>> {
    const  params = new  HttpParams().set('idusuario', idusuario.toString());
    return this.http.get<Correo[]>(`${environment.apiUrl}/index.php/mensajeria/bandeja-de-entrada`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getBandejaDeEntrada', []))
      );
  }

  getBandejaDeEntradaRespuestas(idmensaje: number, numeroLecturas: number, tabla: string, idusuario: number): Observable<any> {
    const params = new HttpParams().set('idmensaje', idmensaje.toString())
                                    .set('numeroLecturas', numeroLecturas.toString())
                                    .set('tabla', tabla)
                                    .set('idusuario', idusuario.toString());
    return this.http.get<any>(`${environment.apiUrl}/index.php/mensajeria/bandeja-de-entrada/respuestas`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getBandejaDeEntradaRespuestas', []))
      );
  }

  getBandejaDeSalida(idusuario: number): Observable<any[] | HttpResponse<Correo[]>> {
    const  params = new  HttpParams().set('idusuario', idusuario.toString());
    return this.http.get<Correo[]>(`${environment.apiUrl}/index.php/mensajeria/bandeja-de-salida`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getBandejaDeSalida', []))
      );
  }

  getBandejaDeSalidaDetalle(idmensaje: number, tabla: string, idusuario: number): Observable<any> {
    const params = new HttpParams().set('idmensaje', idmensaje.toString())
                                    .set('tabla', tabla)
                                    .set('idusuario', idusuario.toString());
    return this.http.get<any>(`${environment.apiUrl}/index.php/mensajeria/bandeja-de-salida/detalle`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getBandejaDeSalidaDetalle', []))
      );
  }

  /**
   * setea la prioridad de un correo
   * @method setPrioridad
   * @param idmensaje - EL id del mensaje
   */
  setPrioridad(idmensaje: number): Observable<any> {
    // const params = new HttpParams().set('idmensaje', idmensaje.toString());
    return this.http.put<any>(`${environment.apiUrl}/index.php/mensajeria/bandeja-de-salida`, idmensaje, {observe: 'response'})
    // return this.http.put<any>(`${environment.apiUrl}/index.php/mensajeria/bandeja-de-salida`, idmensaje, {observe: 'response', params})
      .pipe(
        catchError(this.handleError('setPrioridad', []))
      );
  }

  getUsuariosNuevoCorreo(correorestringido: number, idacceso: number): Observable<any> {
    const params = new HttpParams().set('correorestringido', correorestringido.toString())
                                    .set('idacceso', idacceso.toString());
    return this.http.get<any>(`${environment.apiUrl}/index.php/mensajeria/nuevo-correo`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getUsuariosNuevoCorreo', []))
      );
  }

  getDatosParaSeguimiento(listahijos: string, idacceso: number): Observable<any> {
    const params = new HttpParams().set('listahijos', listahijos)
                                    .set('idacceso', idacceso.toString());
    return this.http.get<any>(`${environment.apiUrl}/index.php/mensajeria/nuevo-correo/seguimiento`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getDatosParaSeguimiento', []))
      );
  }

  postNuevoCorreo(formData: FormData): Observable<any> {
    // const params = new HttpParams().set('idmensaje', idmensaje.toString());
    return this.http.post<any>(`${environment.apiUrl}/index.php/mensajeria/nuevo-correo`, formData, {observe: 'response'})
    // return this.http.put<any>(`${environment.apiUrl}/index.php/mensajeria/bandeja-de-salida`, idmensaje, {observe: 'response', params})
      .pipe(
        catchError(this.handleError('postNuevoCorreo', []))
      );
  }

  getDetalleReenvio(idmensaje: number, tabla: string): Observable<any> {
    const params = new HttpParams().set('idmensaje', idmensaje.toString())
                                    .set('tabla', tabla);
    return this.http.get<any>(`${environment.apiUrl}/index.php/mensajeria/reenviar/detalle`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getDetalleReenvio', []))
      );
  }

  getDetalleResponder(idmensaje: number): Observable<any> {
    const params = new HttpParams().set('idmensaje', idmensaje.toString());
    return this.http.get<any>(`${environment.apiUrl}/index.php/mensajeria/responder`, {params, observe: 'response'})
      .pipe(
        catchError(this.handleError('getDetalleResponder', []))
      );
  }

  postRespuesta(formData: FormData): Observable<any> {
    // const params = new HttpParams().set('idmensaje', idmensaje.toString());
    return this.http.post<any>(`${environment.apiUrl}/index.php/mensajeria/responder`, formData, {observe: 'response'})
      .pipe(
        catchError(this.handleError('postRespuesta', []))
      );
  }

}
