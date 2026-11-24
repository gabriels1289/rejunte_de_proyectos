import { Injectable } from '@angular/core';

import { Observable, of, BehaviorSubject } from 'rxjs';
import { tap, delay, map, catchError } from 'rxjs/operators';

// Lo agrego para consultar a la API
import { HttpClient, HttpParams, HttpHeaders } from '@angular/common/http';
import { HttpErrorHandlerService, handleError } from '../_helpers/http-error-handler.service';
// importo la direccion de la api
import { environment } from '../../environments/environment';
// importo la clase User
import { User } from '../_models/user';
import { response } from 'express';

/* const httpOptions: { headers; observe; } = {
  headers: new HttpHeaders({
    'Content-Type':  'application/json',
  }),
  observe: 'response'
}; */

const httpOptions = {
  headers: new HttpHeaders({
    'Content-Type':  'application/json',
  })
};

@Injectable({
  providedIn: 'root'
})
export class AuthService {
  private currentUserSubject: BehaviorSubject<User>;
  public currentUser: Observable<User>;
  isLoggedIn = false;
  // store the URL so we can redirect after logging in
  redirectUrl: string;
  private handleError: handleError;

  constructor(private http: HttpClient, httpErrorhandler: HttpErrorHandlerService) {
    this.currentUserSubject = new BehaviorSubject<User>(JSON.parse(localStorage.getItem('currentUser')));
    this.currentUser = this.currentUserSubject.asObservable();
    // TODO: eliminar o mejorar el tema de la propiedad isLoggedIn
    if (this.currentUserSubject.value) {
      this.isLoggedIn = true;
    }
    this.handleError = httpErrorhandler.createHandleError('AuthService');
  }

  public get currentUserValue(): User {
    return this.currentUserSubject.value;
  }

  login(usernameParam: string, passwordParam: string) {
    const datos = {
      username: usernameParam,
      password: passwordParam
    };
    const data = JSON.stringify(datos);
    return this.http.post<any>(`${environment.apiUrl}/index.php/login`, data)// , httpOptions)
            .pipe(map(user => {
                // console.log('user: %o', user);
                // login successful if there's a jwt token in the response
                if (user && user.token) {
                    // store user details and jwt token in local storage to keep user logged in between page refreshes
                    localStorage.setItem('currentUser', JSON.stringify(user));
                    this.isLoggedIn = true;
                    this.currentUserSubject.next(user);
                }
                return user;
            }));
  }

  logout(): void {
    // remove user from local storage to log user out
    // TODO: desloguear en DB
    const datos = {
      username: this.currentUserSubject.value.username,
      token: this.currentUserSubject.value.token
    };
    const data = JSON.stringify(datos);
    this.http.put<any>(`${environment.apiUrl}/index.php/logout`, data)// , httpOptions)
            /*.pipe(
                  // console.log('En el pipe del logout'),
                  // catchError(this.handleError('logout', []))
                  map( respuesta => { console.log('respuesta logout: %o', respuesta); return respuesta; } )
            );*/
            .subscribe(
              resp => {
                console.log('PUT Request is successful: %o', resp);
              },
              error => {
                console.log('Error %o', error);
              } // catchError(this.handleError('logout', []))
            );
    localStorage.removeItem('currentUser');
    this.currentUserSubject.next(null);
    this.isLoggedIn = false;
  }

  /*login(): Observable<boolean> {
    return of(true).pipe(
      delay(1000),
      tap(val => this.isLoggedIn = true)
    );
  }*/

}
